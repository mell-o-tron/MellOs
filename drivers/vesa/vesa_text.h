#pragma once
#ifdef VGA_VESA
void _vesa_framebuffer_init(void);

#include "../utils/typedefs.h"
void set_cursor_pos_raw(uint16_t pos);
uint16_t get_cursor_pos_raw();

void clear_screen_col (Colour col);
void clear_line_col(uint32_t line, Colour col);
void kprint_col(const char* s, Colour col);
void kprint(const char* s);
void kprint_char (char c, bool caps);

void move_cursor_LR(int i);
void move_cursor_UD(int i);

void print_error(const char* s);
#endif