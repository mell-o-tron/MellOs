#include "bitmap.h"

void set_bitmap(bitmap_t b, int i) {
    b[i / 8] |= 1 << (i & 7);
}

void unset_bitmap(bitmap_t b, int i) {
    b[i / 8] &= ~(1 << (i & 7));
}

char get_bitmap(bitmap_t b, int i) {
    return b[i / 8] & (1 << (i & 7)) ? 1 : 0;
}

bitmap_t create_bitmap(void* loc, int n) {
    for (int i = 0; i < (n + 7) / 8; i++)
        ((bitmap_t)loc)[i] = 0;
        
    return (bitmap_t)loc;
}
