#include "../../Drivers/VGA_Text.h"
#define FDEF(name) void name(const char* s)

extern uint16_t CursorPos;

FDEF(echo){
    kprint(s+1);
}

FDEF(clear){
    ClearScreen(-1);
    CursorPos = 0;
}