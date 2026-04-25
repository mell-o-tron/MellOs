#include "frame_allocator.h"
#include "dynamic_mem.h"
#include "errno.h"
#include "mellos/kernel/kernel.h"
#include "mellos/kernel/kernel_stdio.h"
#include "mellos/kernel/memory_mapper.h"
#include "mem.h"
#include "paging.h"

__attribute__((
    section(".low.bss"))) static uint32_t early_frame_bitmap[HEAP_SIZE / PHYSICAL_FRAME_SIZE / 32];
__attribute__((section(".low.bss"))) static uint32_t
    early_kernel_frame_bitmap[KERNEL_HEAP_SIZE / PHYSICAL_FRAME_SIZE / 32];

extern uint32_t page_directories[NUM_MANY_DIRECTORIES][1024];
extern uint32_t currently_selected_directory;

extern uint32_t heap_page_table[];
extern uint32_t kernel_heap_page_table[KERNEL_HEAP_PAGE_TABLES][1024];

frame_allocator_t primary_frame_allocator;
frame_allocator_t kernel_frame_allocator;

static frame_allocator_t* get_allocator(bool is_kernel) {
	return is_kernel ? &kernel_frame_allocator : &primary_frame_allocator;
}

static uint32_t* get_page_table(bool is_kernel, uint32_t directory_index) {
	if (is_kernel) {
		return kernel_heap_page_table[directory_index];
	} else {
		return heap_page_table;
	}
}

static bool is_virtual_range_free(bool is_kernel, uint32_t start_dir_index,
                                  uint32_t start_page_table_index, uint32_t frames) {
	uint32_t max_tables = is_kernel ? KERNEL_HEAP_PAGE_TABLES : HEAP_PAGE_TABLES;
	uint32_t dir_index = start_dir_index;
	uint32_t page_table_index = start_page_table_index;

	for (uint32_t i = 0; i < frames; i++) {
		if (dir_index >= max_tables) {
			return false;
		}

		uint32_t* page_table = get_page_table(is_kernel, dir_index);
		if (page_table[page_table_index] & PT_PRESENT) {
			return false;
		}

		page_table_index++;
		if (page_table_index >= 1024) {
			page_table_index = 0;
			dir_index++;
		}
	}
	return true;
}

void* alloc_frame(bool is_kernel, uint32_t continuous_frames_to_alloc) {
	if (continuous_frames_to_alloc == 0) {
		return NULL;
	}

	uint32_t start_physical_address =
	    alloc_physical_frames_ranged(is_kernel, continuous_frames_to_alloc);

	if (errno) {
		if (errno == ENOMEM) {
			kpanic_message("alloc_frame: alloc_physical_frames_ranged: Out Of Memory");
		} else if (errno == EINVAL) {
			kpanic_message(
			    "alloc_frame: alloc_physical_frames_ranged: Invalid Argument (trying to allocate "
			    "0 frames)");
		}
		return NULL;
	}

	PT_FLAGS pt_flags = is_kernel ? PT_PRESENT | PT_READWRITE : PT_PRESENT | PT_READWRITE | PT_USER;
	PD_FLAGS pd_flags = is_kernel ? PD_PRESENT | PD_READWRITE : PD_PRESENT | PD_READWRITE | PD_USER;

	uint32_t virtual_page_directory_entry_start =
	    is_kernel ? KERNEL_HEAP_VIRT_START : HEAP_VIRT_START;
	uint32_t max_tables = is_kernel ? KERNEL_HEAP_PAGE_TABLES : HEAP_PAGE_TABLES;

	uint32_t found_directory_index = 0;
	uint32_t found_page_table_index = 0;
	bool found = false;

	for (uint32_t directory_index = 0; directory_index < max_tables; directory_index++) {
		for (uint32_t page_table_index = 0; page_table_index < 1024; page_table_index++) {
			if (is_virtual_range_free(is_kernel, directory_index, page_table_index,
			                          continuous_frames_to_alloc)) {
				found_directory_index = directory_index;
				found_page_table_index = page_table_index;
				found = true;
				break;
			}
		}
		if (found)
			break;
	}

	if (!found) {
		kpanic_message("Failed to find contiguous virtual frames in heap.");
		return NULL;
	}

	// Map frames
	uint32_t mapping_dir_index = found_directory_index;
	uint32_t mapping_page_table_index = found_page_table_index;
	for (uint32_t i = 0; i < continuous_frames_to_alloc; i++) {
		uint32_t* current_page_table = get_page_table(is_kernel, mapping_dir_index);
		current_page_table[mapping_page_table_index] =
		    (start_physical_address + (i * PHYSICAL_FRAME_SIZE)) | pt_flags;

		mapping_page_table_index++;
		if (mapping_page_table_index >= 1024) {
			mapping_page_table_index = 0;
			mapping_dir_index++;
		}
	}

	// Update Page Directory
	uint32_t literal_virtual_address_pd_start = virtual_page_directory_entry_start >> 22;
	for (uint32_t i = found_directory_index; i <= mapping_dir_index && i < max_tables; i++) {
		uint32_t* page_table_to_map = get_page_table(is_kernel, i);
		put_page_table_to_directory(page_directories[currently_selected_directory],
		                            page_table_to_map, literal_virtual_address_pd_start + i,
		                            pd_flags);
	}

	// Flush TLB
	uint32_t cr3;
	asm volatile("mov %%cr3, %0" : "=r"(cr3));
	asm volatile("mov %0, %%cr3" : : "r"(cr3) : "memory");

	uintptr_t final_virtual_address =
	    (uintptr_t)virtual_page_directory_entry_start +
	    ((uintptr_t)found_directory_index * 1024 * 1024 * 4) + // each PD entry is 4MB
	    ((uintptr_t)found_page_table_index * 4096);

	return (void*)final_virtual_address;
}

/**
 * @brief Initializes the bitmap of a given frame allocator for a specified range.
 *
 * This function validates the frame range of the provided frame allocator
 * and initializes its bitmap. If the start frame is greater than or equal to
 * the end frame, the function logs a panic message and aborts initialization.
 * Otherwise, it clears the memory of the bitmap, preparing it for use.
 *
 * @param frame_allocator A pointer to the `frame_allocator_t` structure
 * responsible for managing a specified range of physical frames.
 */
void init_frame_allocator_range(frame_allocator_t* frame_allocator) {
	if (frame_allocator->start_frame >= frame_allocator->end_frame) {
		kpanic_message("Invalid frame range: min >= max");
		return;
	}

	memset(frame_allocator->frame_bitmap, 0, frame_allocator->bitmap_size);

	/*kprintf("Frame allocator init: range [0x%X, 0x%X) (%u - %u frames)\n",
	        (frame_allocator->start_frame + frame_allocator->offset) * PHYSICAL_FRAME_SIZE,
	        (frame_allocator->end_frame + frame_allocator->offset) * PHYSICAL_FRAME_SIZE,
	        frame_allocator->start_frame, frame_allocator->end_frame);*/
}

/**
 * @brief Initializes the frame allocators for the system.
 *
 * This method sets up and initializes two primary frame allocators:
 * - `primary_frame_allocator` for general heap memory management.
 * - `kernel_frame_allocator` for memory management in the kernel heap.
 *
 * These allocators are configured with specified ranges of physical frames,
 * predefined bitmaps, and their corresponding virtual addresses. After the
 * frame allocators are configured, their initialization function,
 * `init_frame_allocator_range`, is called to prepare them for use.
 *
 * This function must be called during system initialization before any memory
 * allocations using the frame allocators are performed.
 */
void init_frame_allocators() {
	primary_frame_allocator = (frame_allocator_t){heap_phys_start / PHYSICAL_FRAME_SIZE,
	                                              0,
	                                              HEAP_SIZE / PHYSICAL_FRAME_SIZE,
	                                              true,
	                                              early_frame_bitmap,
	                                              sizeof(early_frame_bitmap),
	                                              0};
	kernel_frame_allocator = (frame_allocator_t){kernel_heap_phys_start / PHYSICAL_FRAME_SIZE,
	                                             0,
	                                             KERNEL_HEAP_SIZE / PHYSICAL_FRAME_SIZE,
	                                             false,
	                                             early_kernel_frame_bitmap,
	                                             sizeof(early_kernel_frame_bitmap),
	                                             0};

	init_frame_allocator_range(&primary_frame_allocator);
	init_frame_allocator_range(&kernel_frame_allocator);
}

uint32_t alloc_physical_frame_internal(bool is_kernel, uint32_t start_frame_index,
                                       uint32_t continuous_frames) {
	uint32_t free_count = 0;
	frame_allocator_t* allocator = get_allocator(is_kernel);

	for (uint32_t j = start_frame_index; j < allocator->end_frame && free_count < continuous_frames;
	     j++) {
		uint32_t bitmap_index = j / 32;
		uint32_t bit_index = j % 32;

		if (allocator->frame_bitmap[bitmap_index] & (1 << bit_index)) {
			break;
		}
		free_count++;
	}

	// Found a block of sufficient size
	if (free_count >= continuous_frames) {
		for (uint32_t j = start_frame_index; j < start_frame_index + continuous_frames; j++) {
			uint32_t bitmap_index = j / 32;
			uint32_t bit_index = j % 32;

			allocator->frame_bitmap[bitmap_index] |= (1 << bit_index);
		}
		allocator->next_free_frame = start_frame_index + continuous_frames + 1;
		return (start_frame_index + allocator->offset) * PHYSICAL_FRAME_SIZE;
	}
	return 0;
}

uint32_t alloc_physical_frames_ranged(bool is_kernel, uint32_t frames) {
	if (frames == 0) {
		errno = EINVAL;
		return 0;
	}
	frame_allocator_t* allocator = get_allocator(is_kernel);
	uint32_t ret = 0;

	// Search forward from next_free_frame
	for (uint32_t i = allocator->next_free_frame; i < allocator->end_frame - 1; i++) {
		ret = alloc_physical_frame_internal(is_kernel, i, frames);
		if (!errno) {
			return ret;
		}
	}
	// Wrap around to search from min_frame
	for (uint32_t i = allocator->start_frame; i < allocator->next_free_frame - 1; i++) {
		ret = alloc_physical_frame_internal(is_kernel, i, frames);
		if (!errno) {
			return ret;
		}
	}

	errno = ENOMEM;
	return 0; // todo: defragment memory / oom killer
}

void free_frames_from_allocator(bool is_kernel, uint32_t start_frame, uint32_t frames) {
	frame_allocator_t* allocator = get_allocator(is_kernel);

	for (uint32_t i = start_frame; i < start_frame + frames; i++) {
		uint32_t bitmap_index = i / 32;
		uint32_t bit_index = i % 32;
		allocator->frame_bitmap[bitmap_index] &= ~(1 << bit_index);
	}

	if (start_frame < allocator->next_free_frame) {
		allocator->next_free_frame = start_frame;
	}
}

void free_physical_frame(bool is_kernel, uint32_t physical_address) {
	frame_allocator_t* allocator = get_allocator(is_kernel);

	uint32_t frame_index = (physical_address / PHYSICAL_FRAME_SIZE) - allocator->offset;

	// Validate frame is within range
	if (frame_index >= allocator->end_frame) {
		kprintf("WARNING: Attempted to free physical address 0x%X (index %u) outside allocator "
		        "range (0 to %u)\n",
		        physical_address, frame_index, allocator->end_frame);
		return;
	}

	free_frames_from_allocator(is_kernel, frame_index, 1);
}

void free_frame(bool is_kernel, void* virtual_address, uint32_t frames) {
	if (virtual_address == NULL || frames == 0) {
		return;
	}

	uintptr_t addr = (uintptr_t)virtual_address;
	uint32_t virtual_start_dir_base = is_kernel ? KERNEL_HEAP_VIRT_START : HEAP_VIRT_START;

	uint32_t dir_index = (addr - virtual_start_dir_base) >> 22;
	uint32_t page_table_index = ((addr - virtual_start_dir_base) >> 12) & 0x3FF;

	for (uint32_t i = 0; i < frames; i++) {
		uint32_t* page_table = get_page_table(is_kernel, dir_index);
		uint32_t pte = page_table[page_table_index];

		if (pte & PT_PRESENT) {
			uint32_t physical_address = pte & ~0xFFFu;
			free_physical_frame(is_kernel, physical_address);
			page_table[page_table_index] = 0;

			// Flush TLB for this page
			asm volatile("invlpg (%0)" ::"r"((void*)addr) : "memory");
		}

		addr += PHYSICAL_FRAME_SIZE;
		page_table_index++;
		if (page_table_index >= 1024) {
			page_table_index = 0;
			dir_index++;
		}
	}
}

void switch_to_dynamic_bitmaps(MemoryArea* mmap) {
	if (!mmap || mmap->length == 0) {
		kpanic_message("switch_to_dynamic_bitmaps: Invalid mmap");
	}

	uint32_t new_primary_frames = mmap->length / PHYSICAL_FRAME_SIZE;
	uint32_t new_primary_bitmap_size = (new_primary_frames + 31) / 32 * 4;
	uint32_t* new_primary_bitmap = kzalloc(new_primary_bitmap_size);

	uint32_t* new_kernel_bitmap = kzalloc(kernel_frame_allocator.bitmap_size);

	if (!new_primary_bitmap || !new_kernel_bitmap) {
		kpanic_message("Failed to allocate dynamic bitmaps");
	}

	// Migrate primary allocator state
	// The early bitmap covered [old_start_phys, old_end_phys)
	// We need to map those physical addresses to the new allocator's indices
	uint32_t old_start_phys = (uint32_t)heap_phys_start;
	uint32_t old_end_phys = (uint32_t)heap_phys_end;
	uint32_t new_start_phys = mmap->start;
	uint32_t new_end_phys = mmap->start + mmap->length;

	for (uint32_t phys = old_start_phys; phys < old_end_phys; phys += PHYSICAL_FRAME_SIZE) {
		// If the old frame is within the new range, copy its state
		if (phys >= new_start_phys && phys < new_end_phys) {
			uint32_t old_idx = (phys - old_start_phys) / PHYSICAL_FRAME_SIZE;
			uint32_t new_idx = (phys - new_start_phys) / PHYSICAL_FRAME_SIZE;

			if (primary_frame_allocator.frame_bitmap[old_idx / 32] & (1 << (old_idx % 32))) {
				new_primary_bitmap[new_idx / 32] |= (1 << (new_idx % 32));
			}
		}
	}

	// For kernel allocator, it stays the same range for now, just migrate bitmap
	memcpy(new_kernel_bitmap, kernel_frame_allocator.frame_bitmap,
	       kernel_frame_allocator.bitmap_size);

	// Update primary allocator
	primary_frame_allocator.offset = mmap->start / PHYSICAL_FRAME_SIZE;
	primary_frame_allocator.start_frame = 0;
	primary_frame_allocator.end_frame = new_primary_frames;
	primary_frame_allocator.frame_bitmap = new_primary_bitmap;
	primary_frame_allocator.bitmap_size = new_primary_bitmap_size;
	primary_frame_allocator.next_free_frame = 0;

	// Update kernel allocator
	kernel_frame_allocator.offset = kernel_heap_phys_start / PHYSICAL_FRAME_SIZE;
	kernel_frame_allocator.start_frame = 0;
	kernel_frame_allocator.end_frame = KERNEL_HEAP_SIZE / PHYSICAL_FRAME_SIZE;
	kernel_frame_allocator.frame_bitmap = new_kernel_bitmap;

	kprintf("Switched to dynamic frame bitmaps. Primary range: 0x%X - 0x%X (%u frames)\n",
	        new_start_phys, new_end_phys, new_primary_frames);
}
