#ifdef VGA_VESA

#include "vterm.h"
#include "drivers/vesa/vesa.h"
#include "drivers/vesa/vesa_text.h"
#include "GUI/gui.h"
#include "shell/functions/vell.h"

static Window* w;

void _init_vterm(){
    Framebuffer* fb = _vesa_text_get_framebuffer();
    _vesa_text_set_autoblit(false);
    _vesa_text_set_dirty_callback(_vell_draw);

    w = create_window_with_fb(fb, "vterm");
    w->x = 0;
    w->y = 0;

    // kprint("VTERM FRAMEBUFFER: ");
    // kprint_hex((uint32_t)w->fb);
    // kprint("X: ");
    // kprint_hex(w->x);
    // kprint("Y: ");
    // kprint_hex(w->y);
}

void _deinit_vterm(){
    destroy_window_preserve_fb(w);
    _vesa_text_set_autoblit(true);
    _vesa_text_set_dirty_callback(NULL);
}

#endif