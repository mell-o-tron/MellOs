#ifndef TYPEDEF_H
#define TYPEDEF_H



typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long size_t;

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

typedef uint8_t bool;
#define true    1
#define false   0

#define low_16(address) (uint16_t)((address) & 0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)
#define low_8(address) (uint8_t)((address) &  0xFF)
#define high_8(address) (uint8_t)(((address) >> 8) & 0xFF)

#define NULL (void*)0

typedef struct {
    bool is_some;
    int val;

} maybe_int;

typedef maybe_int maybe_void;

typedef void (*function_type)(void);

#endif
