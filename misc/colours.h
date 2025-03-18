#pragma once
#define BLUE_COLOUR 0x9f
#define DEFAULT_COLOUR 0x0F
#define BAR_COLOUR 0x1E
#define DARK_COLOUR 0x0F
#define ERROR_COLOUR 0x74
#define DARK_INVERSE 0xF0

#include "../utils/typedefs.h"

VESA_Colour vga2vesa(Colour col);

#define VESA_BLACK (VESA_Colour){0, 0, 0, 0xFF}
#define VESA_WHITE (VESA_Colour){0xFF, 0xFF, 0xFF, 0xFF}
#define VESA_RED (VESA_Colour){0xFF, 0, 0, 0xFF}
#define VESA_GREEN (VESA_Colour){0, 0xFF, 0, 0xFF}
#define VESA_BLUE (VESA_Colour){0, 0, 0xFF, 0xFF}
#define VESA_YELLOW (VESA_Colour){0xFF, 0xFF, 0, 0xFF}
#define VESA_CYAN (VESA_Colour){0, 0xFF, 0xFF, 0xFF}
#define VESA_MAGENTA (VESA_Colour){0xFF, 0, 0xFF, 0xFF}
#define VESA_GREY (VESA_Colour){0x80, 0x80, 0x80, 0xFF}
#define VESA_DARK_GREY (VESA_Colour){0x40, 0x40, 0x40, 0xFF}
#define VESA_LIGHT_GREY (VESA_Colour){0xC0, 0xC0, 0xC0, 0xFF}