#include "random.h"
#include "typedefs.h"

static uint32_t seed = 123456789;

uint32_t rand() {
    seed = (1103515245 * seed + 12345) & 0x7fffffff;
    return seed;
}

float frand() {
    return (float)rand() / (float)0x7fffffff;
}