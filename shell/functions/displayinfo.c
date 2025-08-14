#ifdef VGA_VESA
#include "../../drivers/vesa/vesa_text.h"
#else
#include "../../drivers/vga_text.h"
#endif
#include "../../utils/conversions.h"
#define FDEF(name) void name(const char* s)

#define SCALE 16

FDEF(displayinfo){
    kprint("Display Information:\n");
    kprint("Resolution: ");
    kprint(tostring_inplace(Hres, 10));
    kprint("x");
    kprint(tostring_inplace(Vres, 10));
    kprint("\nPitch: ");
    kprint(tostring_inplace(Pitch, 10));
    kprint("\n");
}