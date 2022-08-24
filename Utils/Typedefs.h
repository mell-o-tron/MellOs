#ifndef TYPEDEF_H
#define TYPEDEF_H



typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

typedef uint8_t bool;
#define true    1
#define false   0

#define low_16(address) (uint16_t)((address) & 0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)
#define low_8(address) (uint8_t)((address) &  0xFF)
#define high_8(address) (uint8_t)(((address) >> 8) & 0xFF)

#endif
