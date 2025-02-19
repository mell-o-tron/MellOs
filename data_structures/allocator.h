#pragma once
#include "bitmap.h"
#include "../utils/typedefs.h"

typedef struct {
    bitmap_t bitmap;
    uint32_t size;
} allocator_t;

bitmap_t get_allocator_bitmap (allocator_t * allocator);

void set_alloc_bitmap (allocator_t * allocator, bitmap_t loc, int length);

int allocate (allocator_t * allocator, int n);

int allocator_free(allocator_t * allocator, uint32_t index, int size);
