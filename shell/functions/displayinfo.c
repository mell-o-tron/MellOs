#ifdef VGA_VESA
#include "../../drivers/vesa/vesa_text.h"
#else
#include "../../drivers/vga_text.h"
#endif
#include "../../utils/conversions.h"
#define FDEF(name) void name(const char* s)

#define SCALE 16

FDEF(displayinfo){
#ifdef VGA_VESA
    kprint("Display Information:\n");
#ifdef VGA_VESA
    kprint("Resolution: ");
    kprint(tostring_inplace(Hres, 10));
    kprint("x");
    kprint(tostring_inplace(Vres, 10));
    kprint("\nPitch: ");
    kprint(tostring_inplace(Pitch, 10));
#else
    kprint("Resolution: 80x25 characters");
#endif
    kprint("\n");
#else
    kprint("This command is disabled when the os is not built with the VGA_VESA option.");
#endif
}