#include "frame_allocator.h"
#include "mellos/kernel/kernel.h"
#include "mellos/kernel/kernel_stdio.h"
#include "mem.h"
#include "paging.h"

__attribute__((section(".low.bss"))) static uint32_t
    frame_bitmap[(HEAP_PHYS_END - HEAP_PHYS_START) / PHYSICAL_FRAME_SIZE];
__attribute__((section(".low.bss"))) static uint32_t
    kernel_frame_bitmap[(KERNEL_HEAP_PHYS_END - KERNEL_HEAP_PHYS_START) / PHYSICAL_FRAME_SIZE];

extern uint32_t page_directories[NUM_MANY_DIRECTORIES][1024];
extern uint32_t currently_selected_directory;

extern uint32_t heap_page_table[];
extern uint32_t kernel_heap_page_table[KERNEL_HEAP_PAGE_TABLES][1024];

frame_allocator_t primary_frame_allocator;
frame_allocator_t kernel_frame_allocator;

void* alloc_frame(uint32_t owner, uint32_t continuous_frames_to_alloc) {
	if (continuous_frames_to_alloc == 0) {
		return NULL;
	} else if (continuous_frames_to_alloc > 1024) {
		kpanic_message("Cannot allocate more than 1024 frames at once.");
	}

	uint32_t ret = alloc_physical_frames_ranged(owner == 0, continuous_frames_to_alloc);

	PT_FLAGS pt_flags =
	    owner == 0 ? PT_PRESENT | PT_READWRITE : PT_PRESENT | PT_READWRITE | PT_USER;
	PD_FLAGS pd_flags =
	    owner == 0 ? PD_PRESENT | PD_READWRITE : PD_PRESENT | PD_READWRITE | PD_USER;

	uint32_t virt_offset = owner == 0 ? KERNEL_HEAP_VIRT_START : HEAP_VIRT_START;
	uint32_t phys_offset = owner == 0 ? KERNEL_HEAP_PHYS_START : HEAP_PHYS_START;
	uint32_t max_tables = owner == 0 ? KERNEL_HEAP_PAGE_TABLES : HEAP_PAGE_TABLES;

	uint32_t free_frames_found = 0;

	if (ret != 0) {
		uint32_t virt_addr = virt_offset + ret;
		uint32_t dir_index = (phys_offset + ret) >> 22;
		uint32_t tbl_index = ((phys_offset + ret) >> 12) & 0x3FF;

		while (free_frames_found < continuous_frames_to_alloc) {
			if (tbl_index >= 1024) {
				dir_index++;
				tbl_index = 0;
			}
			if (dir_index > max_tables) {
				return NULL;
			}

			if (kernel_heap_page_table[dir_index][tbl_index] & PT_PRESENT) {
				tbl_index++;
				free_frames_found = 0;
				continue;
			}

			free_frames_found++;
		}

		if (free_frames_found >= continuous_frames_to_alloc) {
			for (uint32_t i = 0; i < continuous_frames_to_alloc; i++) {

				tbl_index = ((ret) >> 12 & 0x3FF) + i % 0x1000;
				dir_index = ((ret) >> 22) + ALIGN_DOWN(i, 0x1000) / 0x1000;
				kernel_heap_page_table[dir_index][tbl_index] =
				    (phys_offset + ret + (i * PHYSICAL_FRAME_SIZE)) | pt_flags;
			}
		} else {
			kpanic_message("Failed to find contiguous physical frames.");
		}

		for (uint32_t i = 0; i < dir_index; i++) {
			put_page_table_to_directory(
			    page_directories[currently_selected_directory],
			    (uint32_t)&kernel_heap_page_table[i],
			    virt_addr >> 12 & 0x3FF, pd_flags);
		}

		// todo: invlpg in case there is only 1 page
		uint32_t cr3;
		asm volatile("mov %%cr3, %0" : "=r"(cr3));
		asm volatile("mov %0, %%cr3" : : "r"(cr3) : "memory");
		//
		// if (!map_run_and_own(owner, dir_index, tbl_index, continuous, phys_offset + ret,
		// pt_flags, flags)) { 	kpanic_message("Failed to own physical acquire physical frames,
		// likely desync between frame allocator and owner manager.");
		// }
		return (void*)virt_addr;
	}

	return NULL;
}

void init_frame_allocator_range(frame_allocator_t* frame_allocator) {
	primary_frame_allocator =
	    (frame_allocator_t){(HEAP_VIRT_START) / PHYSICAL_FRAME_SIZE,
	                        0,
	                        (HEAP_PHYS_END - HEAP_PHYS_START) / PHYSICAL_FRAME_SIZE,
	                        true,
	                        frame_bitmap,
	                        0};
	kernel_frame_allocator =
	    (frame_allocator_t){(KERNEL_HEAP_VIRT_START) / PHYSICAL_FRAME_SIZE,
	                        0,
	                        (KERNEL_HEAP_PHYS_END - KERNEL_HEAP_PHYS_START) / PHYSICAL_FRAME_SIZE,
	                        false,
	                        kernel_frame_bitmap,
	                        0};

	if (frame_allocator->start_frame >= frame_allocator->end_frame) {
		kpanic_message("Invalid frame range: min >= max");
		return;
	}

	memset(frame_allocator->frame_bitmap, 0, sizeof(frame_allocator->frame_bitmap));

	kprintf("Frame allocator init: range [0x%X, 0x%X) (%u - %u frames)\n",
	        (frame_allocator->start_frame + frame_allocator->offset) * PHYSICAL_FRAME_SIZE,
	        (frame_allocator->end_frame + frame_allocator->offset) * PHYSICAL_FRAME_SIZE,
	        frame_allocator->start_frame, frame_allocator->end_frame);
}

void init_frame_allocators() {
	init_frame_allocator_range(&primary_frame_allocator);
	init_frame_allocator_range(&kernel_frame_allocator);
}

uint32_t alloc_physical_frame_internal(bool kernel_call, uint32_t start_frame_index,
                                       uint32_t continuous_frames) {
	uint32_t free_count = 0;
	frame_allocator_t* allocator = kernel_call ? &kernel_frame_allocator : &primary_frame_allocator;

	for (uint32_t j = start_frame_index; j < allocator->end_frame && free_count < continuous_frames;
	     j++) {
		uint32_t index = j / 32;
		uint32_t bit = j % 32;

		if (allocator->frame_bitmap[index] & (1 << bit)) {
			break;
		}
		free_count++;
	}

	// Found a block of sufficient size
	if (free_count >= continuous_frames) {
		for (uint32_t j = start_frame_index; j < start_frame_index + continuous_frames; j++) {
			uint32_t index = j / 32;
			uint32_t bit = j % 32;

			allocator->frame_bitmap[index] |= (1 << bit);
		}
		allocator->next_free_frame = start_frame_index + continuous_frames;
		return start_frame_index * PHYSICAL_FRAME_SIZE;
	}
	return 0;
}

uint32_t alloc_physical_frames_ranged(bool kernel_call, uint32_t frames) {
	if (frames == 0)
		return 0;
	frame_allocator_t* allocator = kernel_call ? &kernel_frame_allocator : &primary_frame_allocator;
	uint32_t ret;

	// Search forward from next_free_frame
	for (uint32_t i = allocator->next_free_frame; i < allocator->end_frame; i++) {
		if ((ret = alloc_physical_frame_internal(kernel_call, i, frames)) != 0) {
			return ret;
		}
	}

	// Wrap around to search from min_frame
	for (uint32_t i = allocator->start_frame; i < allocator->next_free_frame; i++) {
		if ((ret = alloc_physical_frame_internal(kernel_call, i, frames)) != 0) {
			return ret;
		}
	}

	return 0; // todo: defragment memory / oom killer
}

void free_physical_frame(bool kernel_call, uint32_t physical_address) {
	uint32_t frame = ALIGN_DOWN(physical_address, PHYSICAL_FRAME_SIZE) / PHYSICAL_FRAME_SIZE;
	frame_allocator_t* allocator = kernel_call ? &kernel_frame_allocator : &primary_frame_allocator;
	// Validate frame is within range
	if (frame < allocator->start_frame || frame >= allocator->end_frame) {
		kprintf("WARNING: Attempted to free frame 0x%X outside allocator range [0x%X, 0x%X)\n",
		        frame, allocator->start_frame, allocator->end_frame);
		return;
	}

	uint32_t idx = frame / 32;
	uint32_t bit = frame % 32;
	frame_bitmap[idx] &= ~(1 << bit);

	if (frame < allocator->next_free_frame) {
		allocator->next_free_frame = frame;
	}
}