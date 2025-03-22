#pragma once
#ifdef VGA_VESA

void _vell_draw();
void _vell_register_window(Window* w);
void _vell_deregister_window(Window* w);
void _vell_register_mouse(Window* w);
void _vell_deregister_mouse(Window* w);
void _vell_mouse_move(int old_x, int old_y, int new_x, int new_y);

#endif