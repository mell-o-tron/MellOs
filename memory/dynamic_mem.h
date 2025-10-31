#pragma once

#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

void init_allocators();

bool is_buddy_inited();
void* kmalloc(size_t size);
// int kdisintegrate(void* loc, size_t size);
void* krealloc(void* oldloc, size_t oldsize, size_t newsize);
void kfree(void* loc);
