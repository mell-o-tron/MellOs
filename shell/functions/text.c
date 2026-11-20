#include "autoconf.h"
#include "shell/shell.h"
#include "kernel_stdio.h"
#ifdef CONFIG_GFX_VESA
#include "vesa_text.h"
#else
#include "vga_text.h"
#endif
#include "colours.h"
#include "fdef.h"

extern uint16_t cursor_pos;

extern char ker_tty[4000];

FDEF(echo){
    kprintf("%s\n", s);
}

FDEF(clear){
    #ifdef CONFIG_GFX_VESA
    kclear_screen();
    #else
    clear_screen_col(DEFAULT_COLOUR);
    #endif
	set_cursor_pos_raw(0);
	refresh_shell();
}
