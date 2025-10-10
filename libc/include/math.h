#pragma once
#include "stdint.h"

#define CEILING_POS(X) ((X-(int)(X)) > 0 ? (int)(X+1) : (int)(X))
#define CEILING_NEG(X) (int)(X)
#define CEILING(X) ( ((X) > 0) ? CEILING_POS(X) : CEILING_NEG(X) )

typedef struct Vector2i {
    int x;
    int y;
} Vector2i;

typedef struct Recti {
    union{
        Vector2i pos;
        struct{
            int x;
            int y;
        };
    };
    union{
        Vector2i size;
        struct{
            int width;
            int height;
        };
    };
} Recti;

uint32_t ceil_log (uint32_t x, uint32_t base);
float ln (float x, uint32_t order);
float pow_f (float x, uint32_t n);
int abs(int x);
int min(int a, int b);
int max(int a, int b);
int sqrt(int x);

int vector2i_distance(Vector2i a, Vector2i b);
_Bool vector2i_in_recti(Vector2i a, Recti b);
Recti recti_union(Recti a, Recti b);
Recti recti_intersection(Recti a, Recti b);