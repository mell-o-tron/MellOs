#include "includes.h"

typedef void (*l_char_ptr_to_void_r)(char*);
typedef Window* Window_ptr;
typedef VESA_Colour (*l_uint8_t__uint8_t__uint8_t__uint8_t_to_VESA_Colour_r)(uint8_t, uint8_t, uint8_t, uint8_t);
typedef Framebuffer* Framebuffer_ptr;
typedef size_t (*l_Window_ptr_to_size_t_r)(Window*);
typedef VESA_Colour* VESA_Colour_ptr;
typedef Framebuffer* (*l_Window_ptr_to_Framebuffer_ptr_r)(Window*);

size_t get_height (Window*);
Framebuffer* get_fb (Window*);
void sperkaster (char*);
VESA_Colour make_VESA_Colour (uint8_t, uint8_t, uint8_t, uint8_t);
size_t get_width (Window*);

Framebuffer* get_fb(Window* w) {
    Framebuffer* fb = ((Framebuffer_ptr)(0));

    fb = (*w).fb;
    
    return fb;
}

size_t get_width(Window* w) {
    size_t width = ((size_t)(0));

    width = (*w).width;
    
    return width;
}

size_t get_height(Window* w) {
    size_t height = ((size_t)(0));

    height = (*w).height;
    
    return height;
}

VESA_Colour make_VESA_Colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    VESA_Colour* colour = ((VESA_Colour_ptr)(0));

    VESA_Colour vc = (VESA_Colour){ .r = r, .g = g, .b = b, .a = a };
    colour = &vc;
    
    return *colour;
}

void sperkaster(char* s) {
    if (!_vell_is_active()) {
        printf("You need to run vell\n");
        return;
    } else {

    }
    Window* w = create_window_with_size("Sperkaster", ((uint32_t)(400)), ((uint32_t)(100)));
    set_window_dirty(w);
    fb_draw_rect(0, 0, get_width(w), get_height(w), ((size_t)(10)), make_VESA_Colour(((uint8_t)(255)), ((uint8_t)(255)), ((uint8_t)(255)), ((uint8_t)(255))), get_fb(w));
}
