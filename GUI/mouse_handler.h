#pragma once
#ifdef VGA_VESA

#include "../../drivers/mouse.h"

void _init_mouse_handler();
void move_mouse(int x, int y);

#endif