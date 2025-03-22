#ifdef VGA_VESA

#include "../../drivers/vesa/vesa.h"
#include "../../drivers/vesa/vesa_text.h"
#include "../../misc/colours.h"
#include "../../GUI/gui.h"

#include "../../utils/typedefs.h"
#include "../../data_structures/list.h"
#include "../../GUI/vterm.h"
#include "../../GUI/mouse_handler.h"

static List* windows = NULL;

#define FDEF(name) void name(const char* s)

static Framebuffer* fb;

void _vell_draw(){
    fb_clear_screen_col_VESA(VESA_CYAN, *fb);
    
    List* current = windows;
    while(current != NULL){
        Window* w = (Window*)current->data;
        if (w->draw_frame){
            fb_fill_rect(w->x, w->y, w->width + BORDER_WIDTH * 2, TITLEBAR_HEIGHT, VESA_WHITE, *fb);
            fb_draw_rect(w->x, w->y + TITLEBAR_HEIGHT, w->width + BORDER_WIDTH, w->height + BORDER_WIDTH, BORDER_WIDTH, VESA_WHITE, fb);
            fb_draw_string(w->x + BORDER_WIDTH * 2, w->y + BORDER_WIDTH, w->title, VESA_BLACK, 1.8, 1.8, *fb);
            blit_all_at(w->fb, fb, w->x+BORDER_WIDTH, w->y + BORDER_WIDTH + TITLEBAR_HEIGHT);
        }else{
            blit_all_at(w->fb, fb, w->x, w->y);
        }
        current = current->next;
    }

    blit_all_at(fb, vga_fb, 0, 0);
}

FDEF(vell){
    if (fb == NULL) {
        fb = allocate_full_screen_framebuffer();
        fb->fb = kmalloc(fb->width * fb->height * 4);
        _init_vterm();
        _init_mouse_handler();
        _vell_draw();
    } else {
        _deinit_vterm();
        deallocate_framebuffer(fb);
        fb_clear_screen_col_VESA(VESA_BLACK, *vga_fb);
        fb = NULL;
    }
}

void _vell_register_window(Window* w){
    list_append(&windows, w);
    // kprint("Window registered\n");
}

void _vell_deregister_window(Window* w){
    list_remove(&windows, w);
}

#endif