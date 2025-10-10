#ifdef VGA_VESA
#include "gui.h"
#include "drivers/vesa/vesa.h"
#include "shell/functions/vell.h"
#include "memory/dynamic_mem.h"
#include "utils/assert.h"

Window* create_window(const char* title){
    Framebuffer* fb = allocate_full_screen_framebuffer();
    assert(fb != NULL);
    return create_window_with_fb(fb, title);
}

Window* create_window_with_size(const char* title, uint32_t width, uint32_t height){
    Framebuffer* fb = allocate_framebuffer(width, height);
    assert(fb != NULL);
    return create_window_with_fb(fb, title);
}

Window* create_window_with_fb(Framebuffer* fb, const char* title){
    Window* w = kmalloc(sizeof(Window));
    w->fb = fb;
    w->title = title;
    w->width = fb->width;
    w->height = fb->height;
    w->x = 0;
    w->y = 0;
    w->draw_frame = true;
    w->transparent = false;
    w->focused = false;
    _vell_register_window(w);
    return w;
}

void set_window_dirty(Window* w){
    _vell_draw();
}

void destroy_window(Window* w){
    deallocate_framebuffer(w->fb);
    destroy_window_preserve_fb(w);
}

void destroy_window_preserve_fb(Window* w){
    _vell_deregister_window(w);
    kfree(w, sizeof(Window));
}

Recti recti_of_window(Window* w){
    Recti r;
    r.x = w->x;
    r.y = w->y;
    r.width = w->width + BORDER_WIDTH * 2;
    r.height = w->height + BORDER_WIDTH * 2 + TITLEBAR_HEIGHT;
    return r;
}

void set_window_transparent(Window* w, bool transparent){
    w->transparent = transparent;
    w->fb->transparent = transparent;
}

#endif