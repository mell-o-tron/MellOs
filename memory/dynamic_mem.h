#pragma once

#include "../data_structures/bitmap.h"
#include "../data_structures/allocator.h"

void assign_kmallocator(allocator_t* allocator);
void set_dynamic_mem_loc (void *loc);

void * kmalloc (int size);
int kdisintegrate(void* loc, int size);
void* krealloc (void* oldloc, int oldsize, int newsize);

bitmap_t get_kmallocator_bitmap ();
void set_kmalloc_bitmap (bitmap_t loc, int length);
int kfree(void* loc, int size);
