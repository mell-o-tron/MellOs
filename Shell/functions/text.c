#include "../../Drivers/VGA_Text.h"
#define FDEF(name) void name(const char* s)

extern uint16_t CursorPos;

extern char ker_tty[4000];

FDEF(echo){
    kprint(s+1);
}

FDEF(clear){
    clear_tty(-1, ker_tty);
    CursorPos = 0;
    display_tty(ker_tty);
}
