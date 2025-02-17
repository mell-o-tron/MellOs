#include "../../drivers/vga_text.h"
#include "../../misc/colours.h"
#define FDEF(name) void name(const char* s)

extern uint16_t cursor_pos;

extern char ker_tty[4000];

FDEF(echo){
    kprint(s+1); // +1 bc skip whitespace
}

FDEF(clear){
    clear_screen_col(DEFAULT_COLOUR);
    set_cursor_pos_raw(0);
}
