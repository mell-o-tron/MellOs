#ifdef VGA_VESA
#include "../drivers/vesa/vesa_text.h"
#else
#include "../drivers/vga_text.h"
#endif
#include "../misc/colours.h"

extern char ker_tty[4000];

void assert(uint8_t condition){
    if(!condition){
        #ifdef VGA_VESA
        kclear_screen();
        #else
        clear_screen_col(ERROR_COLOUR);
        #endif

        set_cursor_pos_raw(0);
        
        kprint("assert failed");
    }
}
