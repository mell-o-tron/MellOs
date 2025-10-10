#pragma once
#include "stdint.h"

typedef unsigned char uint8_t;

// this should probably be moved somewhere else
typedef uint8_t Colour;
typedef union {
    struct {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    };
    uint32_t val;
} VESA_Colour;

typedef struct Recti Recti; //forward declaration so there is no need to import libc