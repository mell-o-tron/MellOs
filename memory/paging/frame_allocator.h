#pragma once
#include "stdint.h"
#include "stdbool.h"
#include "memory_area_spec.h"
#include "mellos/kernel/memory_mapper.h"

#define PHYSICAL_FRAME_SIZE 4096
#define FRAME_ALIGN_DOWN(x) ((uintptr_t)ALIGN_DOWN(x, FRAME_SIZE))


typedef struct {
	uintptr_t offset;
	uintptr_t start_frame;
	uintptr_t end_frame;
	bool user;
	uint32_t* frame_bitmap;
	uint32_t bitmap_size;
	uint32_t next_free_frame;
} frame_allocator_t;

void init_frame_allocators();
void switch_to_dynamic_bitmaps(MemoryArea* mmap);

/**
 * Allocate frames for processes, primarily for memory allocators
 * todo: free frames
 * @param owner PID of the process that wants to allocate memory
 * @param continuous_frames_to_alloc How many continuous frames to allocate
 * @return Address to start of allocation
 */
void *alloc_frame(bool is_kernel, uint32_t continuous_frames_to_alloc);
/**
 * Initialize the frame allocator with a specific range
 * @param frame_allocator: Frame allocator to initialize
 */
void init_frame_allocator_range(frame_allocator_t *frame_allocator);
/**
 *
 * @param kernel_call
 * @param start_frame_index
 * @param continuous_frames
 * @return address of start relative relative to frame allocator base address
 */
uint32_t alloc_physical_frame_internal(bool kernel_call, uint32_t start_frame_index, uint32_t continuous_frames);
/**
 * todo: implement
 * @param kernel_call
 * @param frame_index
 * @param continuous_frames
 */
void free_frames_from_allocator(bool is_kernel, uint32_t start_frame, uint32_t frames);
void free_frame(bool is_kernel, void* virtual_address, uint32_t frames);

/**
 * @param kernel_call
 * @param frames amount of frames to allocate
 * @return <=0 on error, >0 is count of frames
 */
uint32_t alloc_physical_frames_ranged(bool kernel_call, uint32_t frames);
