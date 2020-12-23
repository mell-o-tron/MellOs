#ifndef TYPEDEF_H
#define TYPEDEF_H



typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

//USEFUL FOR IDT

#define low_16(address) (uint16_t)((address) && 0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) && 0xFFFF)

#endif
