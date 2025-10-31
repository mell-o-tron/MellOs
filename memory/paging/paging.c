#include "paging.h"
#include "conversions.h"
#include "cpu/isr.h"
#include "dynamic_mem.h"
#include "mellos/kernel/kernel.h"
#include "mellos/kernel/kernel_stdio.h"
#include "mem.h"
#include "memory_area_spec.h"
#include "paging_utils.h"
#include "process_memory.h"
#include "processes.h"
#include "stddef.h"
#include "stdint.h"
#include "stdlib.h"
#include "autoconf.h"
#include "mellos/kernel/memory_mapper.h"
#include "vesa_text.h"

#define MAX_FRAMES 262144 // 1GB of physical memory

static uint32_t frame_allocator_min_frame = 0;
static uint32_t frame_allocator_max_frame = MAX_FRAMES;

__attribute__((section(".low.bss")))
uint32_t base_page_directory_low[1024] __attribute__((aligned(4096)));
__attribute__((section(".low.bss")))
uint32_t first_page_table[1024] __attribute__((aligned(4096)));
__attribute__((section(".low.bss")))
uint32_t second_page_table[1024] __attribute__((aligned(4096)));
__attribute__((section(".low.bss")))
uint32_t kernel_heap_page_table[1024] __attribute__((aligned(4096)));
__attribute__((section(".low.bss")))
uint32_t heap_page_table[1024] __attribute__((aligned(4096)));
// uint32_t lots_of_pages[NUM_MANY_DIRECTORIES][1024] __attribute__((aligned(4096)));
#ifdef CONFIG_GFX_VESA
#define NUM_FB_DIRECTORIES (uint32_t)4 // FB is 8100 KB (1920x1080x4)
__attribute__((section(".low.bss")))
uint32_t framebuffer_pages[NUM_FB_DIRECTORIES][1024] __attribute__((aligned(4096)));
#endif

__attribute__((section(".low.bss"))) uint32_t page_table_identity[1024]
    __attribute__((aligned(4096)));
__attribute__((section(".low.bss"))) uint32_t page_table_higher_half[1024] __attribute__((aligned(4096)));

__attribute__((section(".low.bss")))
uint32_t page_directories[NUM_MANY_DIRECTORIES][1024]
    __attribute__((aligned(4096)));
uint32_t currently_selected_directory = 0;

#define KERNEL_START 0xC0000000
#define KERNEL_PHYSICAL_START 0x00100000
#define KERNEL_START_INDEX 768


#define PHYSICAL_FRAME_SIZE 4096

#define PAGE_ALIGN_DOWN(x) ((uintptr_t)(x) & ~(uintptr_t)(PAGE_SIZE - 1))

static uint32_t frame_bitmap[MAX_FRAMES / 32];
static uint32_t next_free_frame = 0;

void init_frame_allocator_range(uint32_t min_frame, uint32_t max_frame) {
	if (min_frame >= max_frame) {
		kpanic_message("Invalid frame range: min >= max");
		return;
	}

	if (max_frame > MAX_FRAMES) {
		kpanic_message("Frame range exceeds MAX_FRAMES");
		return;
	}

	memset(frame_bitmap, 0, sizeof(frame_bitmap));

	// Mark all frames below min_frame as used
	for (uint32_t frame = 0; frame < min_frame; frame++) {
		uint32_t index = frame / 32;
		uint32_t bit = frame % 32;
		frame_bitmap[index] |= (1 << bit);
	}

	// Mark all frames at or above max_frame as used
	for (uint32_t frame = max_frame; frame < MAX_FRAMES; frame++) {
		uint32_t index = frame / 32;
		uint32_t bit = frame % 32;
		frame_bitmap[index] |= (1 << bit);
	}

	// Store the range
	frame_allocator_min_frame = min_frame;
	frame_allocator_max_frame = max_frame;
	next_free_frame = min_frame;

	kprintf("Frame allocator initialized: range [0x%X, 0x%X) (%u - %u frames)\n",
	        min_frame * PHYSICAL_FRAME_SIZE,
	        max_frame * PHYSICAL_FRAME_SIZE,
	        min_frame,
	        max_frame);
}

void init_frame_allocator(void) {
	// Default: mark everything below HEAP_START as used
	uint32_t heap_start_frame = HEAP_START / PHYSICAL_FRAME_SIZE;
	uint32_t heap_end_frame = (KERNEL_HEAP_END + 1) / PHYSICAL_FRAME_SIZE;

	init_frame_allocator_range(heap_start_frame, heap_end_frame);
}

void get_frame_allocator_range(uint32_t* out_min_frame, uint32_t* out_max_frame) {
	if (out_min_frame)
		*out_min_frame = frame_allocator_min_frame;
	if (out_max_frame)
		*out_max_frame = frame_allocator_max_frame;
}

inline uint32_t alloc_physical_frame_internal(uint32_t i, uint32_t frames) {
	uint32_t free_count = 0;

	for (uint32_t j = i; j < frame_allocator_max_frame && free_count < frames; j++) {
		uint32_t index = j / 32;
		uint32_t bit = j % 32;

		if (frame_bitmap[index] & (1 << bit)) {
			break;
		}
		free_count++;
	}

	// Found a block of sufficient size
	if (free_count == frames) {
		for (uint32_t j = i; j < i + frames; j++) {
			uint32_t index = j / 32;
			uint32_t bit = j % 32;

			frame_bitmap[index] |= (1 << bit);
		}
		next_free_frame = i + frames;
		return i * PHYSICAL_FRAME_SIZE;
	}
	return 0;
}

uint32_t alloc_physical_frames_ranged(uint32_t frames) {
	if (frames == 0)
		return 0;

	uint32_t ret;

	// Search forward from next_free_frame
	for (uint32_t i = next_free_frame; i < frame_allocator_max_frame; i++) {
		if ((ret = alloc_physical_frame_internal(i, frames)) != 0) {
			return ret;
		}
	}

	// Wrap around to search from min_frame
	for (uint32_t i = frame_allocator_min_frame; i < next_free_frame; i++) {
		if ((ret = alloc_physical_frame_internal(i, frames)) != 0) {
			return ret;
		}
	}

	return 0; // OOM
}

uint32_t alloc_physical_frames(uint32_t frames) {
	return alloc_physical_frames_ranged(frames);
}

void free_physical_frame(uint32_t physical_address) {
	uint32_t frame = physical_address / PHYSICAL_FRAME_SIZE;

	// Validate frame is within range
	if (frame < frame_allocator_min_frame || frame >= frame_allocator_max_frame) {
		kprintf("WARNING: Attempted to free frame 0x%X outside allocator range [0x%X, 0x%X)\n",
		        frame, frame_allocator_min_frame, frame_allocator_max_frame);
		return;
	}

	uint32_t idx = frame / 32;
	uint32_t bit = frame % 32;
	frame_bitmap[idx] &= ~(1 << bit);

	if (frame < next_free_frame) {
		next_free_frame = frame;
	}
}

// writes the page directory full of tables with write perms
__attribute__((section(".low.text")))
void initialize_page_directory(uint32_t* directory) {
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

static bool run_is_free(uint32_t start_dir, uint32_t start_tbl, uint32_t dir_lo, uint32_t dir_hi, size_t count) {
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
static bool map_run_and_own(uint32_t owner, uint32_t start_dir, uint32_t start_table, size_t count,
                            uint32_t phys_base, uint32_t pte_flags, PD_FLAGS pd_flags) {
	uint32_t dir = start_dir, table = start_table;

	// Ensure user PDEs are marked present where needed.
	if (owner != 0) {
		// We'll ensure PDE present for any directory we touch.
		// Remember the last dir we set to avoid redundant calls.
		bool dir_present = false;
		uint32_t last_dir = 0;
		uint32_t tmpd = start_dir, tmpt = start_table;
		for (size_t k = 0; k < count; ++k) {
			if (tmpd != last_dir) {
				put_page_table_to_directory(page_directories[currently_selected_directory],
				                            page_directories[currently_selected_directory][tmpd],
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
		uint32_t phys = phys_base + (uint32_t)(k * PAGE_SIZE);
		uintptr_t virt_addr = ((uintptr_t)dir << 22) | ((uintptr_t)table << 12);

		((uint32_t *)page_directories[currently_selected_directory][dir])[table] =
		    (phys & ~0xFFFu) | (pte_flags | PT_PRESENT);

		// Track page in process's page list
		if (proc != NULL) {
			if (!process_memory_add_page(proc->page_list, virt_addr)) {
				kprintf("WARNING: Failed to add page %p to process %u's page list\n", virt_addr, owner);
			}
		}

		if (k + 1 < count) {
			(void)next_slot(&dir, &table, start_dir, 1023);
		}
	}

	enablePaging();
	return true;
}

__attribute__((section(".low.text")))
bool allocate_pages_with_offset(uint32_t owner, size_t count, uint32_t offset) {
	if (count == 0)
		return false;
	offset = PAGE_ALIGN_DOWN(offset);

	const uint32_t dir_lo = (owner == 0) ?
		(((uint32_t)KERNEL_HEAP_START >> 12) & 0x3FF) :
		((HEAP_START >> 12) & 0x3FF);
	const uint32_t dir_hi = (owner == 0) ?
		(((uint32_t)KERNEL_HEAP_END >> 12) & 0x3FF) :
		(((HEAP_START + 4096*1024) >> 12) & 0x3FF);

	const uint32_t pte_flags = (owner == 0) ? (PT_PRESENT | PT_READWRITE) : (PT_PRESENT | PT_READWRITE | PT_USER);

	const PD_FLAGS pd_flags =
	    (owner == 0) ? (PD_PRESENT | PD_READWRITE) : (PD_PRESENT | PD_READWRITE | PD_USER);

	for (uint32_t i = dir_lo; i <= dir_hi; ++i) {
		for (uint32_t j = 0; j < 1024; ++j) {
			uint32_t max_pages_left_in_dir = 1024 - j + (dir_hi - i) * 1024;
			if ((size_t)max_pages_left_in_dir < count)
				break;

			if (!run_is_free(i, j, dir_lo, dir_hi, count))
				continue;

			uint32_t phys_base = alloc_physical_frames((uint32_t)count);
			if (phys_base == 0) {
				if (owner == 0) {
					kpanic_message("Out of memory (kernel pages)");
				} else {
					kpanic_message("Out of memory (process pages)");
				}
				return NULL;
			}

			return map_run_and_own(owner, i, j, count, phys_base, pte_flags, pd_flags);
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

bool allocate_page_with_offset(uint32_t owner, size_t count, uint32_t offset) {
	return allocate_pages_with_offset(owner, count, offset);
}

bool allocate_page(uint32_t owner, size_t count) {
	return allocate_pages_with_offset(owner, count, 0);
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
			process_memory_add_page(&proc->page_list, virt_addr);
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
__attribute__((section(".low.text")))
void map_framebuffer_pages(uintptr_t fb_addr) {
#ifdef CONFIG_GFX_VESA
	for (uint32_t i = 0; i < 2; i++) {
		uintptr_t phys_base = (fb_addr & ~0x3FFFFF) + (i * 0x400000);

		for (uint32_t j = 0; j < 1024; j++) {
			framebuffer_pages[i][j] = phys_base + (j * 0x1000) | PT_PRESENT | PT_READWRITE;
		}
		put_page_table_to_directory(base_page_directory_low, (uint32_t)framebuffer_pages[i], 2 + 512 + i,
		                            FRAMEBUFFER_PAGE_DFLAGS);
	}
#endif
}
__attribute__((section(".low.text")))
void put_page_table_to_directory(uint32_t** directory, const unsigned int page_table, uint32_t index,
                            PD_FLAGS pdf) {
	if (index >= 1024) {
		kpanic_message("Invalid page directory index");
	}

	directory[index] = (unsigned int*)((uint32_t)page_table | pdf);
}

__attribute__((optimize("O0"))) __attribute__((noinline))
__attribute__((section(".text")))
_Noreturn void higher_half_init(MultibootTags* multiboot_addr) {
	memset(page_directories, 0, sizeof(page_directories));
	memset(heap_page_table, 0, sizeof(heap_page_table));
	memset(kernel_heap_page_table, 0, sizeof(kernel_heap_page_table));
	for (int i = 0; i < 1024; i++) {
		heap_page_table[i] = (2*0x400000 + i * 0x1000) | (PT_PRESENT | PT_READWRITE | PT_USER);
		kernel_heap_page_table[i] = (3*0x400000 + i * 0x1000) | (PT_PRESENT | PT_READWRITE);
	}

	put_page_table_to_directory(page_directories[0], (uint32_t)first_page_table, 0,
	                            PD_PRESENT | PD_READWRITE);
	put_page_table_to_directory(page_directories[0], (uint32_t)second_page_table, 1,
	                            PD_PRESENT | PD_READWRITE);
	put_page_table_to_directory(page_directories[0], (uint32_t)page_table_higher_half, 768,
	                            PD_PRESENT | PD_READWRITE);
	put_page_table_to_directory(page_directories[0], (uint32_t)heap_page_table, 2,
	                            PD_PRESENT | PD_READWRITE | PD_USER);
	put_page_table_to_directory(page_directories[0], (uint32_t)kernel_heap_page_table, 3,
	                            PD_PRESENT | PD_READWRITE);

#ifdef CONFIG_GFX_VESA
	for (uint32_t i = 0; i < NUM_FB_DIRECTORIES; i++) {
		uintptr_t phys_base_fb = (get_multiboot_framebuffer_addr(multiboot_addr) & ~0x3FFFFF) + (i * 0x400000);

		for (uint32_t j = 0; j < 1024; j++) {
			framebuffer_pages[i][j] = (phys_base_fb + (j * 0x1000)) | PT_PRESENT | PT_READWRITE;
		}
		put_page_table_to_directory(page_directories[0], (uint32_t)framebuffer_pages[i], FRAMEBUFFER_PD_INDEX + i,
									FRAMEBUFFER_PAGE_DFLAGS);
	}
#endif

	switch_page_directory((uint32_t*)page_directories[0]);
	init_frame_allocator();

	higher_half_main((uintptr_t)multiboot_addr); // kernel main for higher half
}

extern char __text_va_start[];
extern char __text_pa_end[];
extern char __text_pa_start[];
extern char __text_delta[];

extern char __data_va_start[];
extern char __data_pa_end[];
extern char __data_pa_start[];

extern char __rodata_va_start[];
extern char __rodata_pa_end[];
extern char __rodata_pa_start[];

extern char __bss_va_start[];
extern char __bss_pa_end[];
extern char __bss_pa_start[];

__attribute__((optimize("O0"))) __attribute__((section(".low.text")))
void setup_paging_with_dual_mapping(uintptr_t fb, MultibootTags* multiboot_info_addr) {
	isrs_install();

	memset(first_page_table, 0, sizeof(first_page_table));
	memset(second_page_table, 0, sizeof(second_page_table));

	for (int i = 0; i < 1024; i++) {
		first_page_table[i] = i * 0x1000 | (PT_PRESENT | PT_READWRITE);
		second_page_table[i] = (0x400000 + i * 0x1000) | (PT_PRESENT | PT_READWRITE);
	}

	memset(base_page_directory_low, 0, sizeof(base_page_directory_low));

	put_page_table_to_directory(base_page_directory_low, (uint32_t)first_page_table, 0,
	                            PD_PRESENT | PD_READWRITE);
	put_page_table_to_directory(base_page_directory_low, (uint32_t)second_page_table, 1,
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
	uint32_t text_delta = __text_pa_end - __text_pa_start;
	uint32_t data_delta = __data_pa_end - __data_pa_start;
	uint32_t rodata_delta = __rodata_pa_end - __rodata_pa_start;
	uint32_t bss_delta = __bss_pa_end - __bss_pa_start;

	uint32_t text_offset = 0;
	uint32_t rodata_offset = text_delta + text_offset;
	uint32_t data_offset = rodata_offset + rodata_delta;
	uint32_t bss_offset = data_offset + data_delta;

	for (int i = 0; i < 1024; i++) {
		if (i >= text_offset / 0x1000 && i < (rodata_offset) / 0x1000) {
			// Map kernel code/data
			page_table_higher_half[i] = ((uint32_t)__text_pa_start + i * 0x1000) | (PT_PRESENT);
		} else if (i >= rodata_offset / 0x1000 && i < (data_offset) / 0x1000) {
			page_table_higher_half[i] = ((uint32_t)__rodata_pa_start + i * 0x1000) | (PT_PRESENT); // readonly, also on paging
		} else if (i >= data_offset / 0x1000 && i < (bss_offset) / 0x1000) {
			page_table_higher_half[i] = ((uint32_t)__data_pa_start + i * 0x1000) | (PT_PRESENT | PT_READWRITE);
		} else if (i >= bss_offset / 0x1000 && i < (bss_offset + bss_delta) / 0x1000) {
			page_table_higher_half[i] = ((uint32_t)__bss_pa_start + i * 0x1000) | (PT_PRESENT | PT_READWRITE);
		} else if (i >= 1020) {
			// Map stack (last 4 pages, 16 KB)
			uint32_t stack_offset = (i - 1020) * 0x1000;
			uint32_t phys_addr = (0x90000 - KERNEL_STACK_SIZE) + stack_offset;
			page_table_higher_half[i] = phys_addr | (PT_PRESENT | PT_READWRITE);
		}
	}

	put_page_table_to_directory(base_page_directory_low, (uint32_t)page_table_higher_half, 768,
	                            PD_PRESENT | PD_READWRITE);

	// base_page_directory[768] = ((unsigned int)page_table_higher_half) | (PD_PRESENT |
	// PD_READWRITE); // Higher-half mapping

	uint32_t old_esp = 0, old_ebp = 0;
	asm volatile("mov %%esp, %0" : "=r"(old_esp));
	asm volatile("mov %%ebp, %0" : "=r"(old_ebp));

	if (old_esp > 0x90000) {
		kpanic_message("ESP is too high");
	}

	uint32_t magic_number = 0x80000000;
	loadPageDirectory(base_page_directory_low);
	asm volatile("mov %%cr0, %%eax\n"
	             "or %0, %%eax\n"
	             "mov %%eax, %%cr0\n" ::"r"(magic_number)
	             : "eax");
#ifdef CONFIG_GFX_VESA
	map_framebuffer_pages(fb);
#endif
	uintptr_t offset_esp = 0x90000 - old_esp;
	uintptr_t new_esp = UPPER_KERNEL_STACK_TOP - offset_esp;
	new_esp &= ~0xF;
	asm volatile("mov %0, %%esp\n" ::"r"(new_esp));
	uintptr_t offset_ebp = 0x90000 - old_ebp;
	uintptr_t new_ebp = UPPER_KERNEL_STACK_TOP - offset_ebp;
	new_ebp &= ~0xF;
	asm volatile("mov %0, %%ebp\n" ::"r"(new_ebp));

	// switch_page_directory(base_page_directory);

	uint32_t cr0_check;
	asm volatile("mov %%cr0, %0\n" : "=r"(cr0_check));

	kprintf("CR0 after paging = %X\n", cr0_check);

	// After enabling paging and adjusting stack
	uint32_t* test_ptr = (uint32_t*)0xC0000000;
	uint32_t test_value = *test_ptr; // Try to read from higher-half
	kprintf("Read from 0xC0000000: %X\n", test_value);
	asm volatile("mov %1, %%eax\n"
	             "jmp *%0\n" // this is set at the top of the function
	             ::"r"(higher_half_init),
	             "r"(multiboot_info_addr)
	             : "memory");
	kpanic_message("Returned from higher_half_init, should not be here");
}
__attribute__((section(".low.text"))) void init_paging(uintptr_t fb,
                                                       MultibootTags* multiboot_info_addr) {
	// Map kernel to higher half
	setup_paging_with_dual_mapping(fb, multiboot_info_addr);
}

void stop_paging() {
	disablePaging();
}

__attribute__((section(".low.text"))) void switch_page_directory(unsigned int* page_directory) {
	loadPageDirectory(page_directory);
	enablePaging();
}