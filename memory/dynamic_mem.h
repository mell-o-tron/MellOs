#pragma once

#include "bitmap.h"
#include "allocator.h"
#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"

void assign_kmallocator(allocator_t* allocator);
void set_dynamic_mem_loc (void *loc);

bool buddy_init(uintptr_t base, size_t size);

void * kmalloc (size_t size);
int kdisintegrate(void* loc, size_t size);
void* krealloc (void* oldloc, size_t oldsize, size_t newsize);

bitmap_t get_kmallocator_bitmap ();
void set_kmalloc_bitmap (bitmap_t loc, uint32_t length);
void kfree(void* loc, size_t size);
