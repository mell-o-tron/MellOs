#ifdef VGA_VESA
#include "../../drivers/vesa/vesa_text.h"
#else
#include "../../drivers/vga_text.h"
#endif
#include "../../misc/colours.h"
#define FDEF(name) void name(const char* s)

extern uint16_t cursor_pos;

extern char ker_tty[4000];

FDEF(echo){
    kprint(s+1); // +1 bc skip whitespace
    kprint("\n");
}

FDEF(clear){
    #ifdef VGA_VESA
    kclear_screen();
    #else
    clear_screen_col(DEFAULT_COLOUR);
    #endif
    set_cursor_pos_raw(0);
}
