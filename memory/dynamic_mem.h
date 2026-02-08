#pragma once

#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

void init_allocators();

extern uintptr_t kernel_heap_phys_start;
extern uintptr_t kernel_heap_phys_end;
extern uintptr_t heap_phys_start;
extern uintptr_t heap_phys_end;

bool is_buddy_inited();
/**
 * allocates memory for kernel purposes
 * @param size bytes to alloc
 * @return start of allocated memory area or NULL if allocation fails
 */
void* kmalloc(size_t size);
/**
 * same as kmalloc but zeroes the memory
 * @param size bytes to alloc
 * @return start of zeroed memory area or NULL if allocation fails
 */
void* kzalloc(size_t size);
// int kdisintegrate(void* loc, size_t size);
void* krealloc(void* oldloc, size_t oldsize, size_t newsize);
void kfree(void* loc);
