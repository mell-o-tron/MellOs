#pragma once

#include "bitmap.h"
#include "allocator.h"
#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"

void init_allocators(void* loc, size_t size);

void * kmalloc (size_t size);
// int kdisintegrate(void* loc, size_t size);
void* krealloc (void* oldloc, size_t oldsize, size_t newsize);
void kfree(void* loc);
