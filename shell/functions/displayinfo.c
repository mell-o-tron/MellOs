#include "autoconf.h"
#ifdef CONFIG_GFX_VESA
#include "vesa_text.h"
#else
#include "vga_text.h"
#endif
#include "conversions.h"
#define FDEF(name) void name(const char* s)

#define SCALE 16

FDEF(displayinfo){
    kprint("Display Information:\n");
#ifdef CONFIG_GFX_VESA
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
}
