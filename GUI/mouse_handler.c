#ifdef VGA_VESA
#include "../utils/typedefs.h"
#include "mouse_handler.h"
#include "gui.h"
#include "../misc/colours.h"
// #include "mouse_icon.h"
#include "../../memory/dynamic_mem.h"
#include "../../shell/functions/vell.h"

Window* mouse_window = NULL;

void _init_mouse_handler(){
    Framebuffer* fb = allocate_framebuffer(16, 16);
    fb->fb = kmalloc(fb->width*fb->height*4);
    mouse_window = create_window_with_fb(fb, "mouse");
    _vell_deregister_window(mouse_window);
    _vell_register_mouse(mouse_window);
    mouse_window->draw_frame = false;
    mouse_window->x = 100;
    mouse_window->y = 200;
    fb_fill_circle(fb->width / 2, fb->height / 2, 6, VESA_BLACK, *mouse_window->fb);
    fb_fill_circle(fb->width / 2, fb->height / 2, 4, VESA_WHITE, *mouse_window->fb);
    // draw_mouse();
    return;
}

void _deinit_mouse_handler(){
    _vell_deregister_mouse(mouse_window);
    destroy_window(mouse_window);
    mouse_window = NULL;
}

// void draw_mouse(){
//     for(int i = 0; i < 16; i++){
//         for(int j = 0; j < 16; j++){
//             uint32_t col = mouse_icon[i][j];
//             mouse_window->fb->fb[i * mouse_window->fb->pitch + j] = col;
//         }
//     }
// }

void move_mouse(int x, int y){
    if (mouse_window == NULL) return;
    int old_x = mouse_window->x;
    int old_y = mouse_window->y;
    mouse_window->x += x;
    if (mouse_window->x < 0) mouse_window->x = 0;
    if (mouse_window->x > vga_fb->width - 16) mouse_window->x = vga_fb->width - 16;
    mouse_window->y += y;
    if (mouse_window->y < 0) mouse_window->y = 0;
    if (mouse_window->y > vga_fb->height - 16) mouse_window->y = vga_fb->height - 16;
    // set_window_dirty(mouse_window);
    _vell_mouse_move(old_x, old_y, mouse_window->x, mouse_window->y);
}

#endif