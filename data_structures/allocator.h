#pragma once
#include "bitmap.h"
#include "../utils/typedefs.h"

typedef struct {
    bitmap_t bitmap;
    uint32_t size;
    size_t granularity;
} allocator_t;

bitmap_t get_allocator_bitmap (allocator_t * allocator);

void set_alloc_bitmap (allocator_t * allocator, bitmap_t loc, uint32_t length);

void* allocate (allocator_t * allocator, size_t n);

int allocator_free(allocator_t * allocator, uint32_t index, size_t size);
