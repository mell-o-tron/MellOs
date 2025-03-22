#pragma once
#ifdef VGA_VESA

#include "../utils/typedefs.h"

#define PIXEL 				uint32_t
// #define FRAMEBUFFER			((volatile PIXEL*)0x800000)

#define BYTES_PER_PIXEL BPP / 8
#define PITCH HRES

typedef struct Framebuffer {
    volatile PIXEL* fb;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
} Framebuffer;

extern Framebuffer* vga_fb;

void _vesa_framebuffer_init(PIXEL pointer);

Framebuffer* allocate_full_screen_framebuffer();
Framebuffer* allocate_framebuffer(uint32_t width, uint32_t height);
void deallocate_framebuffer(Framebuffer* fb);

void blit(Framebuffer src, Framebuffer dest, int x, int y, uint32_t width, uint32_t height);
void blit_all_at(Framebuffer* src, Framebuffer* dest, int x, int y);
void blit_all_at_only(Framebuffer* src, Framebuffer* dest, int x, int y, int from_x, int from_y, int to_x, int to_y);

void clear_screen();
void clear_screen_col_VESA(VESA_Colour col);
void clear_screen_col(Colour col);
void fill_square(int x, int y, int size, VESA_Colour col);
void fill_rect(int x, int y, int width, int height, VESA_Colour col);
void fill_circle(int x, int y, int radius, VESA_Colour col);
void draw_char(uint16_t x, uint16_t y, char c, VESA_Colour colour, float scaleX, float scaleY);

void fb_clear_screen(Framebuffer fb);
void fb_clear_screen_col_VESA(VESA_Colour col, Framebuffer fb);
void fb_clear_screen_col(Colour col, Framebuffer fb);
void fb_fill_square(int x, int y, int size, VESA_Colour col, Framebuffer fb);
void fb_fill_rect(int x, int y, int width, int height, VESA_Colour col, Framebuffer fb);
void fb_fill_circle(int x, int y, int radius, VESA_Colour col, Framebuffer fb);
void fb_draw_char(uint16_t x, uint16_t y, char c, VESA_Colour colour, float scaleX, float scaleY, Framebuffer fb);
void fb_draw_rect(int x, int y, size_t width, size_t height, size_t thickness, VESA_Colour col, Framebuffer* fb);
void fb_draw_string(uint16_t x, uint16_t y, const char* s, VESA_Colour colour, float scaleX, float scaleY, Framebuffer fb);
void fb_draw_gradient(int x, int y, int width, int height, VESA_Colour col1, VESA_Colour col2, Framebuffer* fb);
VESA_Colour vesa_interpolate(VESA_Colour col1, VESA_Colour col2, float ratio);

#endif