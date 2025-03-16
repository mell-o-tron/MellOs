#pragma once
#define BLUE_COLOUR 0x9f
#define DEFAULT_COLOUR 0x0F
#define BAR_COLOUR 0x1E
#define DARK_COLOUR 0x0F
#define ERROR_COLOUR 0x74
#define DARK_INVERSE 0xF0

#include "../utils/typedefs.h"

VESA_Colour vga2vesa(Colour col);