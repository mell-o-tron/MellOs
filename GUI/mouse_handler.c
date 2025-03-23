#ifdef VGA_VESA
#include "../utils/typedefs.h"
#include "mouse_handler.h"
#include "gui.h"
#include "../misc/colours.h"
// #include "mouse_icon.h"
#include "../../memory/dynamic_mem.h"
#include "../../shell/functions/vell.h"
#include "../utils/math.h"

static Window* mouse_window = NULL;

typedef struct ClickState {
    bool is_clicking;
    bool is_dragging;
    Vector2i start_pos;
} ClickState;

typedef struct MouseState {
    Vector2i position;
    ClickState buttons[3];
} MouseState;

static MouseState mouse_state;

void _init_mouse_handler(){
    Framebuffer* fb = allocate_framebuffer(16, 16);
    fb->fb = kmalloc(fb->width*fb->height*4);
    mouse_window = create_window_with_fb(fb, "mouse");
    _vell_deregister_window(mouse_window);
    _vell_register_mouse(mouse_window);
    mouse_window->draw_frame = false;
    mouse_window->x = vga_fb->width / 2;
    mouse_window->y = vga_fb->height / 2;
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
    mouse_state.position.x = mouse_window->x;
    mouse_state.position.y = mouse_window->y;

    for (int i = 0; i < 3; i++){
        if (mouse_state.buttons[i].is_clicking && !mouse_state.buttons[i].is_dragging){
            if (vector2i_distance(mouse_state.position, mouse_state.buttons[i].start_pos) > 30) {
                mouse_state.buttons[i].is_dragging = true;
                _vell_generate_drag_start_event(i, mouse_state.buttons[i].start_pos);
            }
        }
        if (mouse_state.buttons[i].is_dragging){
            _vell_generate_drag_continue_event(i, mouse_state.position);
        }
    }
    // set_window_dirty(mouse_window);
    _vell_mouse_move(old_x, old_y, mouse_window->x, mouse_window->y);
}

void mouse_up(MouseButton button){
    if (mouse_window == NULL) return;

    if (mouse_state.buttons[button].is_dragging){
        _vell_generate_drag_end_event(button, mouse_state.position);
    } else if (mouse_state.buttons[button].is_clicking){
        _vell_generate_click_event(button, mouse_state.position);
    }
    
    mouse_state.buttons[button].is_clicking = false;
    mouse_state.buttons[button].is_dragging = false;
}

void mouse_down(MouseButton button){
    if (mouse_window == NULL) return;

    mouse_state.buttons[button].is_clicking = true;
    mouse_state.buttons[button].start_pos.x = mouse_state.position.x;
    mouse_state.buttons[button].start_pos.y = mouse_state.position.y;
}

#endif