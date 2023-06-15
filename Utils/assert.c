#include "../Drivers/VGA_Text.h"
#include "../Misc/colors.h"

extern char ker_tty[4000];

void assert(int condition){
    if(!condition){
        clear_tty(ERROR_COLOR, ker_tty);
        display_tty(ker_tty);
        SetCursorPosRaw(0);
        
        kprint("assert failed");
    }
}
