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
float fsqrt(float x);

extern const float PI;
extern const float UAT;
extern const float TAU;
float sin(float x);
float cos(float x);
float floor(float x);
float fabs(float x);
double dabs(double x);
float fmod(float x, float y);
float atan_lookup(float x); // arctan lookup table for the CORDIC algorithm
float powf(float x, float n);
float tan(float x);
float fclamp(float x, float min, float max);
uint32_t uclamp(int x, int min, int max);
int clamp(int x, int min, int max);
float fmin(float a, float b);
float fmax(float a, float b);

int vector2i_distance(Vector2i a, Vector2i b);
_Bool vector2i_in_recti(Vector2i a, Recti b);
Recti recti_union(Recti a, Recti b);
Recti recti_intersection(Recti a, Recti b);

// #if defined(__has_builtin)
//   #if __has_builtin(__builtin_popcount)
//     #define USE_BUILTIN_POPCOUNT 1
//   #endif
// #endif
// #ifdef USE_BUILTIN_POPCOUNT
//     #define popcount(x) __builtin_popcount(x)
// #else
    int popcount(uint32_t x);
// #endif