#pragma once

typedef unsigned char* bitmap_t;

void set_bitmap(bitmap_t b, int i);

void unset_bitmap(bitmap_t b, int i);

char get_bitmap(bitmap_t b, int i);

bitmap_t create_bitmap(void* loc, int n);
