#include "paging.h"
#include "autoconf.h"
#include "conversions.h"
#include "cpu/isr.h"
#include "dynamic_mem.h"
#include "frame_allocator.h"
#include "mellos/kernel/kernel.h"
#include "mellos/kernel/kernel_stdio.h"
#include "mellos/kernel/memory_mapper.h"
#include "mem.h"
#include "memory_area_spec.h"
#include "paging_utils.h"
#include "process_memory.h"
#include "processes.h"
#include "stddef.h"
#include "stdint.h"
#include "stdlib.h"
#include "vesa_text.h"

__attribute__((section(".low.bss"))) uint32_t base_page_directory_low[1024]
    __attribute__((aligned(4096)));
__attribute__((section(".low.bss"))) uint32_t first_page_table[1024] __attribute__((aligned(4096)));
__attribute__((section(".low.bss"))) uint32_t second_page_table[1024]
    __attribute__((aligned(4096)));
__attribute__((section(".low.bss"))) uint32_t kernel_heap_page_table[KERNEL_HEAP_PAGE_TABLES][1024]
    __attribute__((aligned(4096)));
__attribute__((section(".low.bss"))) uint32_t heap_page_table[1024] __attribute__((aligned(4096)));
// uint32_t lots_of_pages[NUM_MANY_DIRECTORIES][1024] __attribute__((aligned(4096)));

#ifdef CONFIG_GFX_VESA
#define NUM_FB_DIRECTORIES (uint32_t)4 // FB is 8100 KB (1920x1080x4)
__attribute__((section(".low.bss"))) uint32_t framebuffer_pages[NUM_FB_DIRECTORIES][1024]
    __attribute__((aligned(4096)));

#endif

__attribute__((section(".low.bss"))) uint32_t page_table_higher_half[1024]
    __attribute__((aligned(4096)));

__attribute__((section(".low.bss"))) uint32_t page_directories[NUM_MANY_DIRECTORIES][1024]
    __attribute__((aligned(4096)));
uint32_t currently_selected_directory = 0;

#define KERNEL_START 0xC0000000
#define KERNEL_PHYSICAL_START 0x00100000
#define KERNEL_START_INDEX (KERNEL_START >> 22)

#define PAGE_ALIGN_DOWN(x) ((uintptr_t)ALIGN_DOWN(x, PAGE_SIZE))

extern MultibootTags mb_tags;

// writes the page directory full of tables with write perms
__attribute__((section(".low.text"))) void initialize_page_directory(uint32_t* directory) {
	for (int i = 0; i < 1024; i++) {
		directory[i] = 0;
	}

	// Kernel area
	for (int i = KERNEL_START_INDEX; i < 1024; i++) {
		directory[i] = 0x00000000 | (PD_PRESENT | PD_READWRITE); // Last 3 bits are 011:
		// - U/S = 0 : only the supervisor can access the page
		// - R/W = 1 : the page is read/write
		// - P   = 1 : the page is present (this is now kernel memory)
		// set_page_ownership(base_page_directory, 0, i, 0);
	}
}

bool free_page(pagedata_t* page) {
	if (page == NULL) {
		return false;
	}

	uint32_t page_index = page->page;
	uint32_t page_table = page->page_table_index;

	uint32_t* page_addr = page_index * 0x1000 + page->page_directory[page_table];
	for (int i = 0; i < 1024; i++) {
		page_addr[i] = 0;
	}
	page->page_directory[page_table][page_index] &=
	    ~(PT_PRESENT | PT_READWRITE | PT_USER | PT_DIRTY);

	return true;
}

static inline bool next_slot(uint32_t* dir, uint32_t* tbl, uint32_t dir_lo, uint32_t dir_hi) {
	(*tbl)++;
	if (*tbl >= 1024) {
		*tbl = 0;
		(*dir)++;
		if (*dir > dir_hi)
			return false;
	}
	return true;
}

static bool run_is_free(uint32_t start_dir, uint32_t start_tbl, uint32_t dir_lo, uint32_t dir_hi,
                        size_t count) {
	uint32_t d = start_dir, t = start_tbl;
	for (size_t k = 0; k < count; ++k) {
		if (d < dir_lo || d > dir_hi)
			return false;
		if (((uint32_t*)page_directories[currently_selected_directory][d])[t] & PT_PRESENT)
			return false;
		// No longer checking page_owner_table since we're using per-process tracking

		if (k + 1 < count) {
			if (!next_slot(&d, &t, dir_lo, dir_hi))
				return false;
		}
	}
	return true;
}

// Map `count` pages starting from (dir, tbl), using `phys_base` as the first frame,
// and set proper flags and owners. Returns pagedata for the first page.
bool map_run_and_own(uint32_t owner, uint32_t start_dir, uint32_t start_table, size_t count,
                     uint32_t phys_base, uint32_t pte_flags, PD_FLAGS pd_flags) {
	uint32_t dir = start_dir, table = start_table;

	// Ensure user PDEs are marked present where needed.
	if (owner != 0) {
		// We'll ensure PDE present for any directory we touch.
		// Remember the last dir we set to avoid redundant calls.
		uint32_t last_dir = 0;
		uint32_t tmpd = start_dir, tmpt = start_table;
		for (size_t k = 0; k < count; ++k) {
			if (tmpd != last_dir) {
				put_page_table_to_directory(page_directories[currently_selected_directory],
				                            (uint32_t*)page_directories[currently_selected_directory][tmpd],
				                            tmpd, pd_flags);
				last_dir = tmpd;
			}
			if (k + 1 < count) {
				(void)next_slot(&tmpd, &tmpt, start_dir, 1023); // bounds checked by caller
			}
		}
	}

	process_t* proc = NULL;
	if (owner != 0) {
		proc = get_process_by_pid(owner);
		if (proc == NULL) {
			kprintf("WARNING: Could not find process with PID %u for page tracking\n", owner);
			return false;
		}
	}

	for (uint32_t k = 0; k < count; ++k) {
		uint32_t page_physical_address = phys_base + k * PAGE_SIZE;
		uintptr_t page_relative_virtual_address = ((uintptr_t)dir << 22) | ((uintptr_t)table << 12);

		((uint32_t*)page_directories[currently_selected_directory][dir])[table] =
		    (page_physical_address & ~0xFFFu) | (pte_flags | PT_PRESENT);

		// Track page in process's page list
		if (proc != NULL) {
			if (!process_memory_add_page(proc->page_list, page_relative_virtual_address)) {
				kprintf("WARNING: Failed to add page %p to process %u's page list\n", page_relative_virtual_address,
				        owner);
			}
		}

		if (k + 1 < count) {
			(void)next_slot(&dir, &table, start_dir, 1023);
		}
	}

	load_page_directory(page_directories[currently_selected_directory]);
	return true;
}

__attribute__((section(".low.text"))) bool allocate_pages_with_offset(uint32_t owner, size_t count,
                                                                      uint32_t offset) {
	if (count == 0) {
		return false;
	}
	offset = PAGE_ALIGN_DOWN(offset);

	const uint32_t dir_lo = (owner == 0) ? (((uint32_t)kernel_heap_phys_start >> 12) & 0x3FF)
	                                     : (((uint32_t)heap_phys_start >> 12) & 0x3FF);
	const uint32_t dir_hi = (owner == 0) ? (((uint32_t)kernel_heap_phys_end >> 12) & 0x3FF)
	                                     : ((((uint32_t)heap_phys_start + 4096 * 1024) >> 12) & 0x3FF);

	for (uint32_t i = dir_lo; i <= dir_hi; ++i) {
		for (uint32_t j = 0; j < 1024; ++j) {
			uint32_t max_pages_left_in_dir = 1024 - j + (dir_hi - i) * 1024;
			if ((size_t)max_pages_left_in_dir < count) {
				break;
			}

			if (!run_is_free(i, j, dir_lo, dir_hi, count)) {
				continue;
			}

			void* phys_base = alloc_frame(owner == 0, (uint32_t)count);
			if (phys_base == NULL) {
				if (owner == 0) {
					kpanic_message("Out of memory (kernel pages)");
				} else {
					kpanic_message("Out of memory (process pages)");
				}
				return NULL;
			}

			return phys_base;
		}
	}

	return false;
}

bool allocate_pages_virtual(uint32_t owner, size_t count, uintptr_t addr) {
	if (count == 0)
		return NULL;
	const uint32_t offset = (uint32_t)(addr & (PAGE_SIZE - 1));

	return allocate_pages_with_offset(owner, count, offset);
}

bool allocate_page(uint32_t owner, size_t count) {
	return allocate_pages_with_offset(owner, count, 0);
}

uintptr_t allocate_user_pages_return_base(uint32_t owner, size_t count, uint32_t offset) {
	if (owner == 0 || count == 0)
		return 0;
	offset = PAGE_ALIGN_DOWN(offset);
	const uint32_t dir_lo = (((uint32_t)heap_phys_start >> 12) & 0x3FF);
	const uint32_t dir_hi = ((((uint32_t)heap_phys_start + 4096 * 1024) >> 12) & 0x3FF);

	for (uint32_t i = dir_lo; i <= dir_hi; ++i) {
		for (uint32_t j = 0; j < 1024; ++j) {
			uint32_t max_pages_left_in_dir = 1024 - j + (dir_hi - i) * 1024;
			if ((size_t)max_pages_left_in_dir < count)
				break;
			if (!run_is_free(i, j, dir_lo, dir_hi, count))
				continue;
			void* phys_base = alloc_frame(owner == 0, (uint32_t)count);
			if (phys_base == 0) {
				kpanic_message("Out of memory (process pages)");
				return 0;
			}
			uintptr_t base = ((uintptr_t)i << 22) | ((uintptr_t)j << 12);
			(void)offset; // not currently used in virtual placement
			return base;
		}
	}
	return 0;
}

static inline void invlpg(void* m) {
	asm volatile("invlpg (%0)" ::"r"(m) : "memory");
}

bool free_user_pages(uint32_t owner, uintptr_t base, size_t count) {
	if (owner == 0 || base == 0 || count == 0)
		return false;
	process_t* proc = get_process_by_pid(owner);
	if (!proc || !proc->page_list)
		return false;
	uintptr_t addr = PAGE_ALIGN_DOWN(base);
	uint32_t dir = (uint32_t)(addr >> 22) & 0x3FF;
	uint32_t tbl = (uint32_t)((addr >> 12) & 0x3FF);
	for (size_t k = 0; k < count; ++k) {
		uint32_t* pte_table =
		    (uint32_t*)(page_directories[currently_selected_directory][dir] & ~0xFFFu);
		if (pte_table) {
			uint32_t entry = ((uint32_t*)pte_table)[tbl];
			if (entry & PT_PRESENT) {
				uint32_t phys = entry & ~0xFFFu;
				free_physical_frame(false, phys);
				((uint32_t*)pte_table)[tbl] = 0; // clear mapping
				process_memory_remove_page(proc->page_list,
				                           ((uintptr_t)dir << 22) | ((uintptr_t)tbl << 12));
				invlpg((void*)(((uintptr_t)dir << 22) | ((uintptr_t)tbl << 12)));
			}
		}
		// advance to next slot
		tbl++;
		if (tbl >= 1024) {
			tbl = 0;
			dir++;
		}
	}
	enable_paging();
	return true;
}

extern bool buddy_inited;

pagedata_t* set_page_ownership(unsigned int** page_directory_table,
                               unsigned int page_directory_index, unsigned int page_table_index,
                               unsigned int page_index, unsigned int owner) {

	if (page_directory_index >= NUM_MANY_DIRECTORIES) {
		kpanic_message("Invalid page directory index");
		return NULL;
	}
	if (page_table_index >= 1024) {
		kpanic_message("Invalid page table index");
		return NULL;
	}
	if (page_index >= 1024) {
		kpanic_message("Invalid page index");
		return NULL;
	}

	// For processes, track ownership via process_memory
	if (owner != 0) {
		process_t* proc = get_process_by_pid(owner);
		if (proc != NULL) {
			uintptr_t virt_addr = ((uintptr_t)page_directory_index << 22) |
			                      ((uintptr_t)page_table_index << 12) |
			                      ((uintptr_t)page_index * PAGE_SIZE);
			process_memory_add_page(proc->page_list, virt_addr);
		}
	}

	// Return NULL since we're not using pagedata_t for tracking anymore
	return NULL;
}

// also checks for the directory
bool is_page_present(unsigned int pd_index, unsigned int* page_table, unsigned int page_index) {
	if (base_page_directory_low[pd_index] & PD_PRESENT && page_table[page_index] & PT_PRESENT) {
		return true;
	}
	return false;
}

bool is_page_directory_present(unsigned int pd_index) {
	if ((uint32_t)page_directories[pd_index] & (uint32_t)PD_PRESENT) {
		return true;
	}
	return false;
}
__attribute__((section(".low.text"))) void map_framebuffer_pages(uintptr_t fb_addr) {
#ifdef CONFIG_GFX_VESA
	for (uint32_t i = 0; i < NUM_FB_DIRECTORIES; i++) {
		uintptr_t phys_base = (fb_addr & ~0x3FFFFF) + (i * 0x400000);

		for (uint32_t j = 0; j < 1024; j++) {
			framebuffer_pages[i][j] = phys_base + (j * 0x1000) | PT_PRESENT | PT_READWRITE;
		}
		put_page_table_to_directory(base_page_directory_low, framebuffer_pages[i],
		                            FRAMEBUFFER_PD_INDEX + i, FRAMEBUFFER_PAGE_DFLAGS);
	}
#endif
}
/**
 *
 * @param cr3 Page directory where the table will be inserted
 * @param pte The page table to insert
 * @param pde Index of the page table in the directory, this sets the virtual address of the
 * page table entries (to be inserted at directory[pte])
 * @param pdf Page directory flags
 */
__attribute__((section(".low.text"))) void put_page_table_to_directory(uint32_t* cr3, uint32_t* pte,
                                                                       uint32_t pde, PD_FLAGS pdf) {
	if (pde >= 1024) {
		kpanic_message("Invalid page table index");
	}

	cr3[pde] = (uint32_t)pte | pdf;
}

__attribute__((optimize("O0"))) __attribute__((noinline))
__attribute__((section(".text"))) _Noreturn void
higher_half_init(MultibootTags* multiboot_addr) {

	memset(page_directories, 0, sizeof(page_directories));
	memset(heap_page_table, 0, sizeof(heap_page_table));
	memset(kernel_heap_page_table, 0, sizeof(kernel_heap_page_table));

	uint32_t heap_pages = HEAP_SIZE / 0x1000; // Calculate actual page count

	for (uint32_t i = 0; i < heap_pages; i++) {
		heap_page_table[i] = ((uint32_t)heap_phys_start + i * 0x1000) | (PT_PRESENT | PT_READWRITE | PT_USER);
	}

	uint32_t kernel_heap_pd_entry = KERNEL_HEAP_VIRT_START >> 22;

	for (uint32_t i = 0; i < KERNEL_HEAP_PAGE_TABLES; i++) {
		for (uint32_t j = 0; j < 1024; j++) {
			kernel_heap_page_table[i][j] =
			    (kernel_heap_phys_start + i * 0x400000 + j * 0x1000) | PT_READWRITE;
		}

		// Map each page table into the page directory
		put_page_table_to_directory(page_directories[0], kernel_heap_page_table[i],
		                            kernel_heap_pd_entry + i, PD_PRESENT | PD_READWRITE);
	}

	put_page_table_to_directory(page_directories[0], first_page_table, 0,
	                            PD_PRESENT | PD_READWRITE);
	put_page_table_to_directory(page_directories[0], second_page_table, 1,
	                            PD_PRESENT | PD_READWRITE);
	put_page_table_to_directory(page_directories[0], page_table_higher_half, 768,
	                            PD_PRESENT | PD_READWRITE);
	put_page_table_to_directory(page_directories[0], heap_page_table, HEAP_VIRT_START >> 22,
	                            PD_PRESENT | PD_READWRITE | PD_USER);

#ifdef CONFIG_GFX_VESA
	for (uint32_t i = 0; i < NUM_FB_DIRECTORIES; i++) {
		uintptr_t phys_base_fb =
		    (get_multiboot_framebuffer_addr(&mb_tags) & ~0x3FFFFF) + (i * 0x400000);

		for (uint32_t j = 0; j < 1024; j++) {
			framebuffer_pages[i][j] = (phys_base_fb + (j * 0x1000)) | PT_PRESENT | PT_READWRITE;
		}
		put_page_table_to_directory(page_directories[0], framebuffer_pages[i],
		                            FRAMEBUFFER_PD_INDEX + i, FRAMEBUFFER_PAGE_DFLAGS);
	}
#endif

	switch_page_directory(page_directories[0]);
	isrs_install();
	irq_install();

	init_frame_allocators();

	higher_half_main((uintptr_t)multiboot_addr); // kernel main for higher half
}

extern char __text_va_start[];
extern char __text_pa_end[];
extern char __text_pa_start[];
extern char __text_va_end[];

extern char __data_va_start[];
extern char __data_pa_end[];
extern char __data_pa_start[];
extern char __data_va_end[];

extern char __rodata_va_start[];
extern char __rodata_va_end[];
extern char __rodata_pa_start[];
extern char __rodata_pa_end[];

extern char __bss_va_start[];
extern char __bss_va_end[];
extern char __bss_pa_start[];
extern char __bss_pa_end[];

__attribute__((optimize("O0"))) __attribute__((section(".low.text"))) void
setup_paging_with_dual_mapping(uintptr_t fb, MultibootTags* multiboot_info_addr) {
	isrs_install();
	memset(first_page_table, 0, sizeof(first_page_table));
	memset(second_page_table, 0, sizeof(second_page_table));

	for (int i = 0; i < 1024; i++) {
		first_page_table[i] = i * 0x1000 | (PT_PRESENT | PT_READWRITE);
		second_page_table[i] = (0x400000 + i * 0x1000) | (PT_PRESENT | PT_READWRITE);
	}
	// zero page
	first_page_table[0] = 0;

	memset(base_page_directory_low, 0, sizeof(base_page_directory_low));

	put_page_table_to_directory(base_page_directory_low, first_page_table, 0,
	                            PD_PRESENT | PD_READWRITE);
	put_page_table_to_directory(base_page_directory_low, second_page_table, 1,
	                            PD_PRESENT | PD_READWRITE);
	/*
	    mov eax, 0x0         ; osdev wiki example
	    mov ebx, 0x100000
	    .fill_table:
	        mov ecx, ebx
	        or ecx, 3
	        mov [table_768+eax*4], ecx
	        add ebx, 4096
	        inc eax
	        cmp eax, 1024
	        jne .fill_table
	 */
	uint32_t text_start = ALIGN_DOWN((uint32_t)__text_pa_start, PAGE_SIZE);
	uint32_t text_va_start = (uint32_t)__text_va_start;

	uint32_t data_start = ALIGN_DOWN((uint32_t)__data_pa_start, PAGE_SIZE);

	uint32_t rodata_start = ALIGN_DOWN((uint32_t)__rodata_pa_start, PAGE_SIZE);

	uint32_t bss_start = ALIGN_DOWN((uint32_t)__bss_pa_start, PAGE_SIZE);
	uint32_t bss_va_end = ALIGN_UP((uint32_t)__bss_va_end, PAGE_SIZE);

	for (int page_iterator = 0; page_iterator < 1024; page_iterator++) {
		if (page_iterator >= (text_start - text_start) / 0x1000 &&
		    page_iterator < (rodata_start - text_start) / 0x1000) {
			// Map kernel code/data
			page_table_higher_half[page_iterator] =
			    (text_start + page_iterator * 0x1000) | (PT_PRESENT);
		} else if (page_iterator >= (rodata_start - text_start) / 0x1000 &&
		           page_iterator < (data_start - text_start) / 0x1000) {
			page_table_higher_half[page_iterator] =
			    (text_start + page_iterator * 0x1000) | (PT_PRESENT); // readonly, also on paging
		} else if (page_iterator >= (data_start - text_start) / 0x1000 &&
		           page_iterator < (bss_start - text_start) / 0x1000) {
			page_table_higher_half[page_iterator] =
			    (text_start + page_iterator * 0x1000) | (PT_PRESENT | PT_READWRITE);
		} else if (page_iterator >= (bss_start - text_start) / 0x1000 &&
		           page_iterator < (bss_va_end - text_va_start) / 0x1000) {
			page_table_higher_half[page_iterator] =
			    (text_start + page_iterator * 0x1000) | (PT_PRESENT | PT_READWRITE);
		} else if (page_iterator >= (1024 - (KERNEL_STACK_SIZE / 0x1000))) {
			uint32_t stack_offset =
			    (page_iterator - (1024 - (KERNEL_STACK_SIZE / 0x1000))) * 0x1000;
			uint32_t phys_addr = (0x90000 - KERNEL_STACK_SIZE) + stack_offset;
			page_table_higher_half[page_iterator] = phys_addr | (PT_PRESENT | PT_READWRITE);
		}
	}

	put_page_table_to_directory(base_page_directory_low, page_table_higher_half, 768,
	                            PD_PRESENT | PD_READWRITE);

	// base_page_directory[768] = ((unsigned int)page_table_higher_half) | (PD_PRESENT |
	// PD_READWRITE); // Higher-half mapping

	// marked as volatile cuz i dont trust the compiler
	volatile uint32_t old_esp = 0, old_ebp = 0;
	asm volatile("mov %%esp, %0" : "=r"(old_esp));
	asm volatile("mov %%ebp, %0" : "=r"(old_ebp));

	if (old_esp > PHYSICAL_KERNEL_STACK_TOP) {
		kpanic_message("ESP is too high");
	}

	uint32_t enable_paging_bit = 0x80000001;
	load_page_directory(base_page_directory_low);
	asm volatile("xor %%eax, %%eax\n"
	             "mov %%cr0, %%eax\n"
	             "or %0, %%eax\n"
	             "mov %%eax, %%cr0\n" ::"r"(enable_paging_bit)
	             : "eax");
#ifdef CONFIG_GFX_VESA
	map_framebuffer_pages(fb);
#endif
	uintptr_t offset_esp = PHYSICAL_KERNEL_STACK_TOP - old_esp;
	uintptr_t new_esp = UPPER_KERNEL_STACK_TOP - offset_esp;
	new_esp &= ~0xF;
	asm volatile("mov %0, %%esp\n" ::"r"(new_esp));
	uintptr_t offset_ebp = PHYSICAL_KERNEL_STACK_TOP - old_ebp;
	uintptr_t new_ebp = UPPER_KERNEL_STACK_TOP - offset_ebp;
	new_ebp &= ~0xF;
	asm volatile("mov %0, %%ebp\n" ::"r"(new_ebp));

	// switch_page_directory(base_page_directory);

	uint32_t cr0_check;
	asm volatile("mov %%cr0, %0\n" : "=r"(cr0_check));

	// todo: stack could be cleaned here?
	asm volatile("mov %1, %%eax\n"
	             "jmp *%0\n" // this is set at the top of the function
	             ::"r"(higher_half_init),
	             "r"(multiboot_info_addr)
	             : "memory");
}
__attribute__((section(".low.text"))) void init_paging(uintptr_t fb,
                                                       MultibootTags* multiboot_info_addr) {
	// Map kernel to higher half
	setup_paging_with_dual_mapping(fb, multiboot_info_addr);
}

void stop_paging() {
	disable_paging();
}

__attribute__((section(".low.text"))) void switch_page_directory(unsigned int* page_directory) {
	load_page_directory(page_directory);
	enable_paging();
}
