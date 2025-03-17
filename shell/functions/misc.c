#ifdef VGA_VESA
#include "../../drivers/vesa/vesa_text.h"
#else
#include "../../drivers/vga_text.h"
#endif
#define FDEF(name) void name(const char* s)

FDEF(hcf){
    int x = *((uint8_t*) 0xffffffff) = 1;
}
