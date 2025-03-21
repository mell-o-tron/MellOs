#ifdef VGA_VESA
#include "gui.h"
#include "../drivers/vesa/vesa.h"
#include "../functions/vell.h"
#include "../memory/dynamic_mem.h"

Window* create_window(const char* title){
    Framebuffer* fb = allocate_full_screen_framebuffer();
    create_window_with_fb(fb, title);
}

Window* create_window_with_fb(Framebuffer* fb, const char* title){
    Window* w = kmalloc(sizeof(Window));
    w->fb = fb;
    w->title = title;
    w->width = fb->width;
    w->height = fb->height;
    w->x = 0;
    w->y = 0;
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

#endif