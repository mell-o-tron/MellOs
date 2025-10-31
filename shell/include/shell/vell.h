#pragma once
#include "autoconf.h"
#ifdef CONFIG_GFX_VESA

#include "math.h"
#include "gui.h"
#include "mouse_handler.h"

bool _vell_is_active();
void _vell_draw();
void _vell_register_window(Window* w);
void _vell_deregister_window(Window* w);
void _vell_register_mouse(Window* w);
void _vell_deregister_mouse(Window* w);
void _vell_mouse_move(int old_x, int old_y, int new_x, int new_y);

void _vell_generate_drag_start_event(MouseButton button, Vector2i start_pos);
void _vell_generate_drag_continue_event(MouseButton button, Vector2i current_pos);
void _vell_generate_drag_end_event(MouseButton button, Vector2i end_pos);
void _vell_generate_click_event(MouseButton button, Vector2i click_pos);

#endif