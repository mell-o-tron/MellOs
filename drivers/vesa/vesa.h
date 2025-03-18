#pragma once
#ifdef VGA_VESA

#include "../utils/typedefs.h"

#define PIXEL 				uint32_t
// #define FRAMEBUFFER			((volatile PIXEL*)0x800000)

#define HRES 1920
#define VRES 1080
#define BPP 32
#define BYTES_PER_PIXEL BPP / 8
#define PITCH HRES

typedef struct Framebuffer {
    volatile PIXEL* fb;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
} Framebuffer;

extern Framebuffer vga_fb;

void _vesa_framebuffer_init(PIXEL pointer);

Framebuffer allocate_full_screen_framebuffer();
Framebuffer allocate_framebuffer(uint32_t width, uint32_t height);
void deallocate_framebuffer(Framebuffer fb);

void blit(Framebuffer src, Framebuffer dest, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

void clear_screen();
void clear_screen_col_VESA(VESA_Colour col);
void clear_screen_col(Colour col);
void draw_square(int x, int y, int size, VESA_Colour col);
void draw_circle(int x, int y, int radius, VESA_Colour col);
void draw_char(uint16_t x, uint16_t y, char c, VESA_Colour colour, size_t scaleX, size_t scaleY);

void fb_clear_screen(Framebuffer fb);
void fb_clear_screen_col_VESA(VESA_Colour col, Framebuffer fb);
void fb_clear_screen_col(Colour col, Framebuffer fb);
void fb_draw_square(int x, int y, int size, VESA_Colour col, Framebuffer fb);
void fb_draw_circle(int x, int y, int radius, VESA_Colour col, Framebuffer fb);
void fb_draw_char(uint16_t x, uint16_t y, char c, VESA_Colour colour, size_t scaleX, size_t scaleY, Framebuffer fb);

#endif