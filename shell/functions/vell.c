#include "math.h"
#ifdef VGA_VESA

#include "shell/vell.h"
#include "vesa.h"
#include "vesa_text.h"
#include "colours.h"
#include "circular_list.h"
#include "dynamic_mem.h"
#include "gui.h"
#include "vterm.h"
#include "mouse_handler.h"

static CircularList* windows = NULL;
static Window* mouse_window = NULL;

#define FDEF(name) void name(const char* s)

static Framebuffer* fb;
static Framebuffer* fb2;

static void swap_framebuffers(){
    Framebuffer* tmp = fb;
    fb = fb2;
    fb2 = tmp;
}

#define FOCUSED_COLOUR VESA_CYAN
#define UNFOCUSED_COLOUR VESA_WHITE

void __vell_draw(int from_x, int from_y, int to_x, int to_y){
    Recti bounds;
    bounds.x = from_x;
    bounds.y = from_y;
    bounds.width = to_x - from_x;
    bounds.height = to_y - from_y;


    /* Uncomment the following lines to make every draw call a full draw */
    // Recti all_bounds;
    // all_bounds.x = 0;
    // all_bounds.y = 0;
    // all_bounds.width = fb->width;
    // all_bounds.height = fb->height;
    // bounds = all_bounds;
    // from_x = 0;
    // from_y = 0;
    // to_x = fb->width;
    // to_y = fb->height;
    
    fb_draw_gradient_at_only(0, 0, fb->width, fb->height, VESA_MAGENTA, VESA_CYAN, fb, bounds);

    CircularList* current = windows;
    if (current != NULL){
        do{
            Window* w = (Window*)current->data;
            if (w->draw_frame){
                fb_fill_rect_at_only(w->x, w->y, w->width + BORDER_WIDTH * 2, TITLEBAR_HEIGHT, w->focused ? FOCUSED_COLOUR : UNFOCUSED_COLOUR, fb, bounds);
                fb_draw_rect_at_only(w->x, w->y + TITLEBAR_HEIGHT, w->width + BORDER_WIDTH, w->height + BORDER_WIDTH, BORDER_WIDTH, w->focused ? FOCUSED_COLOUR : UNFOCUSED_COLOUR, fb, bounds);
                fb_draw_string(w->x + BORDER_WIDTH * 2, w->y + BORDER_WIDTH, w->title, VESA_BLACK, 1.8, 1.8, fb);
                blit_all_at_only(w->fb, fb, w->x+BORDER_WIDTH, w->y + BORDER_WIDTH + TITLEBAR_HEIGHT, from_x, from_y, to_x, to_y);
            }else{
                blit_all_at_only(w->fb, fb, w->x, w->y, from_x, from_y, to_x, to_y);
            }
            current = current->next;
        } while(current != windows);
    }
    
    blit_all_at_only(fb, vga_fb, 0, 0, from_x, from_y, to_x, to_y);
    if (mouse_window != NULL){
        blit_all_at_only(mouse_window->fb, vga_fb, mouse_window->x, mouse_window->y, from_x, from_y, to_x, to_y);
    }
    swap_framebuffers();
}

void _vell_draw(){
    __vell_draw(0, 0, vga_fb->width, vga_fb->height);
}

FDEF(vell){
    if (fb == NULL) {
        fb = allocate_full_screen_framebuffer();
        fb2 = allocate_full_screen_framebuffer();
        _init_vterm();
        ((Window*)windows->data)->focused = true;
        _init_mouse_handler();
        _vell_draw();
    } else {
        _deinit_mouse_handler();
        _deinit_vterm();
        deallocate_framebuffer(fb);
        deallocate_framebuffer(fb2);
        fb_clear_screen_col_VESA(VESA_BLACK, vga_fb);
        fb = NULL;
        fb2 = NULL;
        while (windows != NULL){
            Window* w = (Window*)windows->data;
            _vell_deregister_window(w);
            // destroy_window(w); // TODO: Find out why vterm is also destroyed
        }
    }
}

void _vell_register_window(Window* w){
    clist_append(&windows, w);
    // kprint("Window registered\n");
    // kprint("Window: ");
    // kprint(w->title);
}

void _vell_deregister_window(Window* w){
    clist_remove(&windows, w);
}

void _vell_register_mouse(Window* w){
    mouse_window = w;
}

void _vell_deregister_mouse(Window* w){
    mouse_window = NULL;
}

void _vell_mouse_move(int old_x, int old_y, int new_x, int new_y){
    blit_all_at_only(fb2, vga_fb, 0, 0, old_x, old_y, old_x + 16, old_y + 16);
    blit_all_at(mouse_window->fb, vga_fb, new_x, new_y); // No need to do partial blit, as the mouse fb is just 16x16
}

Window* find_window_at(Vector2i pos, WindowElement* element){
    CircularList* current = windows->prev;
    if (current != NULL){
        do{
            Window* w = (Window*)current->data;
            if (vector2i_in_recti(pos, recti_of_window(w))){
                if (element != NULL){
                    if (pos.y < w->y + TITLEBAR_HEIGHT){
                        *element = TITLEBAR;
                    }else if (pos.y > w->y + w->height + BORDER_WIDTH){
                        *element = BORDER_BOTTOM;
                    }else if (pos.x < w->x + BORDER_WIDTH){
                        *element = BORDER_LEFT;
                    }else if (pos.x > w->x + w->width + BORDER_WIDTH){
                        *element = BORDER_RIGHT;
                    }else{
                        *element = CLIENT_AREA;
                    }
                }
                return w;
            }
            current = current->prev;
        } while(current->next != windows);
    }
    return NULL;
}

Window* dragging_window = NULL;
Vector2i dragging_prev_pos;
uint8_t drag_counter = 0;

void __unfocus_all(){
    CircularList* current = windows;
    if (current != NULL) {
        do {
            Window* win = (Window*)current->data;
            win->focused = false;
            current = current->next;
        } while (current != windows);
    }
}

void _vell_generate_drag_start_event(MouseButton button, Vector2i start_pos){
    if(button == MOUSE_LEFT){
        WindowElement element;
        Window* w = find_window_at(start_pos, &element);
        if (w == NULL || element != TITLEBAR) {
            return;
        }

        __unfocus_all();
        w->focused = true;

        dragging_window = w;
        dragging_prev_pos = start_pos;
    }
}

#define GHOST_BORDER_THICKNESS 4
Vector2i ghost_window_delta = {0, 0};
Recti last_square = {};

void _vell_generate_drag_continue_event(MouseButton button, Vector2i current_pos) {
    if (button == MOUSE_LEFT) {
        if (dragging_window == NULL) {
            return;
        }
        /* Uncomment these lines to enable the drag counter, only handling dragging events once every n times */
        // if(drag_counter == 0 || 1){
            int dx = current_pos.x - dragging_prev_pos.x;
            int dy = current_pos.y - dragging_prev_pos.y;
            Recti r = recti_of_window(dragging_window);
#ifndef BOX_WINDOW_DRAG
            dragging_window->x += dx;
            dragging_window->y += dy;
            Recti r2 = recti_of_window(dragging_window);
            dragging_prev_pos = current_pos;
            Recti r3 = recti_intersection(recti_union(r, r2), recti_of_framebuffer(vga_fb));
            __vell_draw(r3.pos.x, r3.pos.y, r3.pos.x + r3.size.x, r3.pos.y + r3.size.y);
#elif BOX_WINDOW_DRAG
            ghost_window_delta.x += dx;
            ghost_window_delta.y += dy;

                Recti ghost_square = {
                    {dragging_window->x + ghost_window_delta.x, dragging_window->y + ghost_window_delta.y},
                    {dragging_window->width + BORDER_WIDTH * 2, dragging_window->height + BORDER_WIDTH * 2 + TITLEBAR_HEIGHT}
                };
                

                blit_all_at_only_square(fb2, vga_fb, 0, 0, last_square, GHOST_BORDER_THICKNESS * 2);
                last_square = ghost_square;
                dragging_prev_pos = current_pos;
                
                fb_draw_rect_at_only(ghost_square.x, ghost_square.y,
                                     ghost_square.size.x, ghost_square.size.y,
                                     GHOST_BORDER_THICKNESS, VESA_DARK_GREY, vga_fb, recti_of_framebuffer(vga_fb));

                swap_framebuffers();
            
#endif      
        // }
        // drag_counter = (drag_counter + 1) % 3;
    }
}

void _vell_generate_drag_end_event(MouseButton button, Vector2i end_pos){
    if (button == MOUSE_LEFT) {
#ifdef BOX_WINDOW_DRAG
        dragging_window->x += ghost_window_delta.x;
        dragging_window->y += ghost_window_delta.y;

        ghost_window_delta.x = 0;
        ghost_window_delta.y = 0;
#endif
        dragging_window = NULL;
        drag_counter = 0;
        _vell_draw();
    }
}

void _vell_generate_click_event(MouseButton button, Vector2i click_pos){
    WindowElement element;
    Window* w = find_window_at(click_pos, &element);

    __unfocus_all();

    if (w != NULL) {
        w->focused = true;
    }
    _vell_draw();
}

bool _vell_is_active(){
    return fb != NULL;
}

#endif