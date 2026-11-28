#pragma once
#include "autoconf.h"
#ifndef CONFIG_GFX_VESA
#include "graphics_types.h"
#include "stdbool.h"
void set_cursor_pos_raw(uint16_t pos);
uint16_t get_cursor_pos_raw();

void clear_screen_col (Colour col);
void clear_line_col(uint32_t line, Colour col);
int kprint_col(const char* s, Colour col);
int kprint(const char* s);
void kprint_char (char c, bool caps);
void kprint_dec(uint32_t n);
void kprint_hex(uint32_t n);

void move_cursor_LR(int i);
void move_cursor_UD(int i);

void print_error(const char* s);
#endif