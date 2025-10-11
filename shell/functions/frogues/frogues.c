#include "vesa.h"
#ifdef VGA_VESA
#include "vesa_text.h"
#define FDEF(name) void name(const char* s)

#include "shell/vell.h"
#include "gui.h"
#include "dynamic_mem.h"
#include "frogues_img.h"
#include "colours.h"

#define SCALE 1

void draw_xbm_scaled(unsigned char* bits, int width, int height, int scale, VESA_Colour fg_color, VESA_Colour bg_color, Framebuffer* fb, Recti bounds) {
    int bytes_per_row = (width + 7) / 8;  // Round up to nearest byte

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Calculate which byte and bit within that byte
            int byte_index = y * bytes_per_row + (x / 8);
            int bit_index = x % 8;

            // XBM format stores bits in LSB first order
            bool pixel_set = (bits[byte_index] >> bit_index) & 1;

            VESA_Colour color = pixel_set ? fg_color : bg_color;

            // Draw scaled pixel block using the VESA API
            int draw_x = x * scale;
            int draw_y = y * scale;

            fb_fill_rect_at_only(draw_x, draw_y, scale, scale, color, fb, bounds);
        }
    }
}

FDEF(frogues){
    if(_vell_is_active()){
        Window* w = create_window_with_size("Frogues", frogues_width * SCALE, frogues_height * SCALE);

        draw_xbm_scaled(frogues_bits, frogues_width, frogues_height, SCALE,
                       VESA_BLACK, VESA_GREEN, w->fb, recti_of_framebuffer(w->fb));

        set_window_dirty(w);
    }
    else{
        kprint("You need to be running vell to view les frogues\n");
    }
}
#endif