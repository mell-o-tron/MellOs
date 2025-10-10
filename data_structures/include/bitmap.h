#pragma once

#include "stdint.h"

typedef unsigned char* bitmap_t;

void set_bitmap(bitmap_t b, size_t i);

void unset_bitmap(bitmap_t b, size_t i);

char get_bitmap(bitmap_t b, size_t i);

bitmap_t create_bitmap(void* loc, size_t n);
