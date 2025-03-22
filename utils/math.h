#pragma once
#include "typedefs.h"

#define CEILING_POS(X) ((X-(int)(X)) > 0 ? (int)(X+1) : (int)(X))
#define CEILING_NEG(X) (int)(X)
#define CEILING(X) ( ((X) > 0) ? CEILING_POS(X) : CEILING_NEG(X) )

uint32_t ceil_log (uint32_t x, uint32_t base);
float ln (float x, uint32_t order);
float pow_f (float x, uint32_t n);
int abs(int x);
int min(int a, int b);
int max(int a, int b);