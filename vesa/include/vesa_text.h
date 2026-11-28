#pragma once
#include "autoconf.h"
#ifdef CONFIG_GFX_VESA

#include "vesa.h"
#include "graphics_types.h"
#include "stddef.h"

void _vesa_text_init();
void _vesa_text_set_framebuffer(Framebuffer* f);
Framebuffer* _vesa_text_get_framebuffer();
void _vesa_text_set_autoblit(bool enabled);
void _vesa_text_set_dirty_callback(function_type f);

int32_t write_draw_buffer(const char*);
int32_t draw_buffer(void);

void set_cursor_pos_raw(uint16_t pos);
uint16_t get_cursor_pos_raw();

void clear_line_col(uint32_t line, Colour col);

void kclear_screen();
int kprint_col(const char* s, Colour col);
int kprint(const char* s);
void kprint_char (char c, bool caps);
void kprint_dec(uint32_t n);
void kprint_hex(uint32_t n);

void move_cursor_LR(int i);
void move_cursor_UD(int i);

void print_error(const char* s);
#endif