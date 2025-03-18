#pragma once
#ifdef VGA_VESA
#include "../utils/typedefs.h"
void _vesa_text_init();

void set_cursor_pos_raw(uint16_t pos);
uint16_t get_cursor_pos_raw();

void kclear_screen();
void kprint_col(const char* s, Colour col);
void kprint(const char* s);
void kprint_char (char c, bool caps);

void move_cursor_LR(int i);
void move_cursor_UD(int i);

void print_error(const char* s);
#endif