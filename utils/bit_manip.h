#pragma once

#define LO_8(x) (x & 0xFF)
#define HI_8(x) ((x >> 8) & 0xFF)

#define SET_FLAG(x,y) (x | (1 << y))
#define CLR_FLAG(x,y) (x & ~(1 << y))
#define CHK_FLAG(x,y) ((x >> y) & 1)
