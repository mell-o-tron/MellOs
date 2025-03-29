#pragma once

#include "../data_structures/bitmap.h"
#include "../data_structures/allocator.h"

void assign_kmallocator(allocator_t* allocator);
void set_dynamic_mem_loc (void *loc);

void * kmalloc (size_t size);
int kdisintegrate(void* loc, size_t size);
void* krealloc (void* oldloc, size_t oldsize, size_t newsize);

bitmap_t get_kmallocator_bitmap ();
void set_kmalloc_bitmap (bitmap_t loc, uint32_t length);
uint32_t kfree(void* loc, size_t size);
