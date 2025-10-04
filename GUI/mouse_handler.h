#pragma once
#ifdef VGA_VESA

#include "../drivers/mouse.h"

typedef enum MouseButton {
    MOUSE_LEFT = 0,
    MOUSE_RIGHT = 1,
    MOUSE_MIDDLE = 2
} MouseButton;

void _init_mouse_handler();
void _deinit_mouse_handler();
void move_mouse(int x, int y);
void mouse_up(MouseButton button);
void mouse_down(MouseButton button);

#endif