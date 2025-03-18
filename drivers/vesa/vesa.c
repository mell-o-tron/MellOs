#ifdef VGA_VESA

#include "vesa.h"
#include "test_font.h"
#include "../utils/typedefs.h"
#include "../misc/colours.h"
#include "../utils/conversions.h"
#include "../memory/dynamic_mem.h"
#include "../memory/mem.h"

Framebuffer vga_fb;
static uint8_t bytes_per_pixel;

void _vesa_framebuffer_init(PIXEL addr){
	vga_fb.fb = (volatile PIXEL*)addr;
    vga_fb.width = HRES;
    vga_fb.height = VRES;
    vga_fb.pitch = PITCH;

	clear_screen(vga_fb);
}

void fb_clear_screen(Framebuffer fb) {
    fb_clear_screen_col_VESA(VESA_BLACK, fb);
}

void clear_screen() {
    fb_clear_screen(vga_fb);
}

void fb_clear_screen_col_VESA(VESA_Colour col, Framebuffer fb){
    uint32_t offset = 0;
    for (int i = 0; i < fb.height - 1 /*TODO: Remove -1 as below*/; i++) {
        offset += fb.pitch; // TODO: This goes at the end of the loop, once memory issues are figured out
        for (int j = 0; j < fb.width; j++) {
            fb.fb[offset + j] = col.val;
        }
    }
}

void clear_screen_col_VESA(VESA_Colour col) {
    fb_clear_screen_col_VESA(col, vga_fb);
}

void fb_clear_screen_col(Colour col, Framebuffer fb) {
    VESA_Colour vesa_col = vga2vesa(col);
    fb_clear_screen_col_VESA(vesa_col, fb);
}

void clear_screen_col(Colour col) {
    fb_clear_screen_col(col, vga_fb);
}

// TODO: readd these functions once memory problems are figured out

// void fb_draw_square(int x, int y, int size, VESA_Colour col, Framebuffer fb){
// 	for (int i = 0; i < size; i++) {
// 		for (int j = 0; j < size; j++) {
// 			fb.fb[(y + i) * fb.pitch + (x + j)] = col.val;
// 		}
// 	}
// }

// void draw_square(int x, int y, int size, VESA_Colour col) {
//     fb_draw_square(x, y, size, col, vga_fb);
// }

// void fb_draw_circle(int x, int y, int radius, VESA_Colour col, Framebuffer fb) {
// 	for (int i = -radius; i < radius; i++) {
// 		for (int j = -radius; j < radius; j++) {
// 			if (i * i + j * j < radius * radius) {
// 				fb.fb[(y + i) * fb.pitch + (x + j)] = col.val;
// 			}
// 		}
// 	}
// }

// void draw_circle(int x, int y, int radius, VESA_Colour col) {
//     fb_draw_circle(x, y, radius, col, vga_fb);
// }

void fb_draw_char(uint16_t x, uint16_t y, char c, VESA_Colour colour, size_t scaleX, size_t scaleY, Framebuffer fb) {
    uint8_t* font_char = font8x8_basic[(uint8_t)c];
    for (int row = 0; row < FONT_HEIGHT * scaleY; row++) {
        uint8_t pixels = font_char[row / scaleY];
        for (int col = 0; col < FONT_WIDTH * scaleX; col++) {
            if (pixels & (1 << (col / scaleX))) { // Check if the bit is set
                fb.fb[((y + row) * fb.pitch) + (x + col)] = colour.val;
            }
        }
    }
}

void draw_char(uint16_t x, uint16_t y, char c, VESA_Colour colour, size_t scaleX, size_t scaleY) {
    fb_draw_char(x, y, c, colour, scaleX, scaleY, vga_fb);
}

Framebuffer allocate_framebuffer(uint32_t width, uint32_t height) {
    uint32_t size = width * height * bytes_per_pixel;
    Framebuffer out = {kmalloc(size), width, height, width /*pitch*/};
    // fb_clear_screen(out);
    return out;
}

Framebuffer allocate_full_screen_framebuffer() {
    return allocate_framebuffer(HRES, VRES);
}

void deallocate_framebuffer(Framebuffer fb) {
    kfree((void*)fb.fb, fb.width * fb.height * bytes_per_pixel);
}

void blit(Framebuffer src, Framebuffer dest, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    if (width < 0 || height < 0 || x > dest.width || y > dest.height) return;
    
    x = x < 0 ? 0 : x;
    y = y < 0 ? 0 : y;

    width = width > dest.width ? dest.width : width;
    height = height > dest.height ? dest.height : height;
    
    // for (uint32_t i = 0; i < height; i++) {
    //     dest.fb[i * src.pitch + 10] = 0xFF00FFFF;
    //     memcp((void*)&src.fb[i * src.pitch], (void*)&dest.fb[(y + i) * dest.pitch + x], width * bytes_per_pixel);
    // }
    uint32_t count = 0;
    uint32_t dest_offset = y * dest.pitch + x;
    uint32_t src_offset = 0;
    for (uint32_t i = 0; i < height; i++) {
        for (uint32_t j = 0; j < width; j++) {
            // count++;
            // if (count > 0x120000) return; 
            dest.fb[dest_offset + j] = src.fb[src_offset + j];
        }
        dest_offset += dest.pitch;
        src_offset += src.pitch;
    }
}

#endif