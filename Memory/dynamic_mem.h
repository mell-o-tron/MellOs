#pragma once

#include "../Utils/bitmap.h"

void set_alloc_bitmap (bitmap_t loc, int length);

void set_dynamic_mem_loc (void *loc);

void *kmalloc (int n);

int kfree(void* loc, int size);

int kdisintegrate(void* loc, int size);

void* krealloc (void* oldloc, int oldsize, int newsize);

bitmap_t get_allocation_bitmap();
