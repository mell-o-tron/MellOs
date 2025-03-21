#ifdef VGA_VESA

#include "../../drivers/vesa/vesa.h"
#include "../../drivers/vesa/vesa_text.h"
#include "../../misc/colours.h"
#include "../../GUI/gui.h"

#include "../../utils/typedefs.h"
#include "../../data_structures/list.h"
#include "../../GUI/vterm.h"

static List* windows = NULL;

#define FDEF(name) void name(const char* s)

static Framebuffer* fb;

void _vell_draw(){
    fb_clear_screen_col_VESA(VESA_CYAN, *fb);
    // blit_all_at(fb, vga_fb, 0, 0);
    
    List* current = windows;
    while(current != NULL){
        // kprint("Drawing window\n");
        Window* w = (Window*)current->data;
        // kprint("Current: ");
        // kprint_hex((uint32_t)current);
        // kprint("Data: ");
        // kprint_hex((uint32_t)w);
        // kprint("X: ");
        // kprint_hex(w->x);
        // kprint("Y: ");
        // kprint_hex(w->y);
        // kprint("FB: ");
        // kprint_hex((uint32_t)w->fb);
        fb_fill_rect(w->x, w->y, w->width + BORDER_WIDTH * 2, TITLEBAR_HEIGHT, VESA_WHITE, *fb);
        fb_draw_rect(w->x, w->y + TITLEBAR_HEIGHT, w->width + BORDER_WIDTH, w->height + BORDER_WIDTH, BORDER_WIDTH, VESA_WHITE, fb);
        fb_draw_string(w->x + BORDER_WIDTH * 2, w->y + BORDER_WIDTH, w->title, VESA_BLACK, 1.8, 1.8, *fb);
        blit_all_at(w->fb, fb, w->x+BORDER_WIDTH, w->y + BORDER_WIDTH + TITLEBAR_HEIGHT);
        // blit(*w->fb, *fb, w->x, w->y, w->width, w->height);
        // kprint("Window drawn\n");
        current = current->next;
    }

    blit_all_at(fb, vga_fb, 0, 0);
}

FDEF(vell){
    if (fb == NULL) {
        fb = allocate_full_screen_framebuffer();
        fb->fb = kmalloc(fb->width * fb->height * 4);
        // char buf[256];
        // tostring(fb->fb, 16, buf);
        // kprint(buf);

        // kprint_hex((uint32_t)windows);
        // kprint("\n");
        _init_vterm();
        // kprint("Here!\n");
        // kprint_hex((uint32_t)windows);
        // kprint_hex((uint32_t)windows->data);
        // kprint_hex(((Window*)windows->data)->x);
        // kprint_hex(((Window*)windows->data)->y);
        // kprint("\n");
        _vell_draw();
        // print_all(&windows);
    
        // print_all(windows);
        // list_append(&windows, 10);
        // print_all(windows);
        // list_append(&windows, 20);
        // print_all(windows);
        // list_append(&windows, 30);
        // print_all(windows);
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