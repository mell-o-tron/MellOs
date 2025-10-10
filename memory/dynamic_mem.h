#pragma once

#include "../data_structures/bitmap.h"
#include "../data_structures/allocator.h"

void init_allocators(void* loc, size_t size);

void * kmalloc (size_t size);
// int kdisintegrate(void* loc, size_t size);
void* krealloc (void* oldloc, size_t oldsize, size_t newsize);
void kfree(void* loc);
