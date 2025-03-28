#pragma once
#ifdef VGA_VESA

#include "../utils/typedefs.h"
#include "../drivers/vesa/vesa.h"
#include "../../utils/math.h"

#define BORDER_WIDTH 5
#define TITLEBAR_HEIGHT 20

typedef struct Window {
    Framebuffer* fb;
    char* title;
    uint32_t width;
    uint32_t height;
    int x;
    int y;
    bool draw_frame;
    bool transparent;
    bool focused;
} Window;

Window* create_window(const char* title);
Window* create_window_with_size(const char* title, uint32_t width, uint32_t height);
Window* create_window_with_fb(Framebuffer* fb, const char* title);
void destroy_window(Window* w);
void destroy_window_preserve_fb(Window* w);

void set_window_transparent(Window* w, bool transparent);
void set_window_dirty(Window* w);

Recti recti_of_window(Window* w);

#endif