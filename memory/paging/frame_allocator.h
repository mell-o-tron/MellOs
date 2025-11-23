#pragma once
#include "stdint.h"
#include "stdbool.h"
#include "memory_area_spec.h"

#define PHYSICAL_FRAME_SIZE 4096
#define FRAME_ALIGN_DOWN(x) ((uintptr_t)ALIGN_DOWN(x, FRAME_SIZE))


typedef struct {
  uintptr_t offset;
  uintptr_t start_frame;
  uintptr_t end_frame;
  bool user;
  uint32_t *frame_bitmap;
  uint32_t bitmap_size;
  uint32_t next_free_frame;
} frame_allocator_t;

/**
 * Low memory frames, each frame is <code>PHYSICAL_FRAME_SIZE</code> bytes
 * This is only meant for the user frames
 */
__attribute__((section(".low.bss")))
static uint32_t frame_bitmap[(HEAP_PHYS_END - HEAP_PHYS_START) / PHYSICAL_FRAME_SIZE];
__attribute__((section(".low.bss")))
static uint32_t kernel_frame_bitmap[(KERNEL_HEAP_PHYS_END - KERNEL_HEAP_PHYS_START) / PHYSICAL_FRAME_SIZE];


void init_frame_allocators();

/**
 * Allocate frames for processes, primarily for memory allocators
 * todo: free frames
 * @param owner PID of the process that wants to allocate memory
 * @param continuous_frames_to_alloc How many continuous frames to allocate
 * @return Address to start of allocation
 */
void *alloc_frame(uint32_t owner, uint32_t continuous_frames_to_alloc);
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
 * @return bytes
 */
uint32_t alloc_physical_frame_internal(bool kernel_call, uint32_t start_frame_index, uint32_t continuous_frames);
/**
 * todo: implement
 * @param kernel_call
 * @param frame_index
 * @param continuous_frames
 */
void free_physical_frame_internal(bool kernel_call, uint32_t frame_index, uint32_t continuous_frames);

/**
 *
 * @param kernel_call
 * @param frames
 * @return bytes
 */
uint32_t alloc_physical_frames_ranged(bool kernel_call, uint32_t frames);