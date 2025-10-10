#include "vesa.h"
#ifdef VGA_VESA
#include "vesa_text.h"
#define FDEF(name) void name(const char* s)

#include "shell/vell.h"
#include "gui.h"
#include "dynamic_mem.h"
#include "frogues_img.h"
#include "colours.h"

#define SCALE 16

FDEF(frogues){
    if(_vell_is_active()){
        Window* w = create_window_with_size("Frogues", frogues_width * SCALE, frogues_height * SCALE);

        uint32_t* fbptr = kmalloc(w->fb->width * w->fb->height * 4);
        if (fbptr == NULL) {
            kprint("Malloc error");
            destroy_window(w);
            return;
        }
        w->fb->fb = fbptr;
        fb_clear_screen(w->fb);

        /*for (int y = 0; y < frogues_height; y++) {
            for (int x = 0; x < frogues_width; x++) {
            int pixel = (frogues_bits[y * frogues_width + x / 8] >> (7 - (x % 8))) & 1;
            VESA_Colour color = pixel ? VESA_WHITE : VESA_BLACK; // White for 1, Black for 0
            fb_fill_rect(x * SCALE, y * SCALE, SCALE, SCALE, color, *w->fb);
            }
        }*/
        fb_fill_rect_at_only(10, 10, 20, 20, VESA_WHITE, *w->fb, recti_of_framebuffer(w->fb));
        set_window_dirty(w);
    }
    else{
        kprint("You need to be running vell to view les frogues\n");
    }
}
#endif