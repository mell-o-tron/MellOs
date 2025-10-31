#include "autoconf.h"
/* Basic Framebuffer graphic library
 * Part of MellOS
 * - Perk
 */

#ifdef CONFIG_GFX_VESA

#include "vesa.h"
#include "test_font.h"
#include "stdint.h"
#include "colours.h"
#include "dynamic_mem.h"
#include "mem.h"

// libc includes
#include "math.h"

#include <mellos/kernel/kernel.h>

uint32_t Hres;
uint32_t Vres;
uint32_t Pitch;

Framebuffer* vga_fb;

void _vesa_framebuffer_init(PIXEL addr){
    vga_fb = kmalloc(sizeof(Framebuffer));
	if (vga_fb == NULL) {
		kpanic_message("Failed to allocate memory for framebuffer\n");
	}
	vga_fb->fb = (volatile PIXEL*)addr;
    vga_fb->width = Hres;
    vga_fb->height = Vres;
    vga_fb->pitch = Pitch;

	clear_screen();
}

void fb_clear_screen(Framebuffer* fb) {
    fb_clear_screen_col_VESA(VESA_BLACK, fb);
}

void clear_screen() {
    fb_clear_screen(vga_fb);
}

void fb_clear_screen_col_VESA(VESA_Colour col, Framebuffer* fb){
    if (fb == NULL) {
        return;
    }
    uint32_t offset = 0;
    for (int i = 0; i < fb->height /*TODO: Remove -1 as below*/; i++) {
        for (int j = 0; j < fb->width; j++) {
            fb->fb[offset + j] = col.val;
        }
        offset += fb->pitch; // TODO: This goes at the end of the loop, once memory issues are figured out
    }
}

void clear_screen_col_VESA(VESA_Colour col) {
    fb_clear_screen_col_VESA(col, vga_fb);
}

void fb_clear_screen_col(Colour col, Framebuffer* fb) {
    VESA_Colour vesa_col = vga2vesa(col);
    fb_clear_screen_col_VESA(vesa_col, fb);
}

void clear_screen_col(Colour col) {
    fb_clear_screen_col(col, vga_fb);
}

void fb_draw_line_at_only(int x1, int y1, int x2, int y2, size_t thickness, VESA_Colour col, Framebuffer* fb, Recti bounds){
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    float Xinc = dx / (float)steps;
    float Yinc = dy / (float)steps;
    float X = x1;
    float Y = y1;
    for (int i = 0; i <= steps; i++) {
        fb_fill_rect_at_only(X, Y, thickness, thickness, col, fb, bounds);
        X += Xinc;
        Y += Yinc;
    }
}

void fb_draw_rect_at_only(int x, int y, size_t width, size_t height, size_t thickness, VESA_Colour col, Framebuffer* fb, Recti bounds){
    fb_draw_line_at_only(x, y, x + width, y, thickness, col, fb, bounds);
    fb_draw_line_at_only(x, y, x, y + height, thickness, col, fb, bounds);
    fb_draw_line_at_only(x + width, y, x + width, y + height, thickness, col, fb, bounds);
    fb_draw_line_at_only(x, y + height, x + width, y + height, thickness, col, fb, bounds);
}

void fb_draw_rect(int x, int y, size_t width, size_t height, size_t thickness, VESA_Colour col, Framebuffer* fb) {
    fb_draw_rect_at_only(x, y, width, height, thickness, col, fb, recti_of_framebuffer(fb));
}

void fb_fill_square(int x, int y, int size, VESA_Colour col, Framebuffer* fb){
	fb_fill_rect(x, y, size, size, col, fb);
}

void fill_square(int x, int y, int size, VESA_Colour col) {
    fb_fill_rect(x, y, size, size, col, vga_fb);
}

void fb_fill_rect_at_only(int x, int y, int width, int height, VESA_Colour col, Framebuffer* fb, Recti bounds) {
    if (x < 0) {
        width += x;
        x = 0;
    }
    if (y < 0) {
        height += y;
        y = 0;
    }
    width = min(width, fb->width - x);
    height = min(height, fb->height - y);

    // width = min(width, bounds.width);
    // height = min(height, bounds.height);
    x = max(x, bounds.x);
    y = max(y, bounds.y);
    width = min(width, bounds.width);
    height = min(height, bounds.height);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fb->fb[(y + i) * fb->pitch + (x + j)] = col.val;
        }
    }
}

void fb_fill_rect(int x, int y, int width, int height, VESA_Colour col, Framebuffer* fb) {
    fb_fill_rect_at_only(x, y, width, height, col, fb, recti_of_framebuffer(fb));
}

void fill_rect(int x, int y, int width, int height, VESA_Colour col) {
    fb_fill_rect(x, y, width, height, col, vga_fb);
}

void fb_fill_circle(int x, int y, int radius, VESA_Colour col, Framebuffer* fb) {
	for (int i = -radius; i < radius; i++) {
		for (int j = -radius; j < radius; j++) {
			if (i * i + j * j < radius * radius) {
				fb->fb[(y + i) * fb->pitch + (x + j)] = col.val;
			}
		}
	}
}

void fill_circle(int x, int y, int radius, VESA_Colour col) {
    fb_fill_circle(x, y, radius, col, vga_fb);
}

void fb_draw_char(uint16_t x, uint16_t y, char c, VESA_Colour colour, float scaleX, float scaleY, Framebuffer* fb) {
    uint8_t* font_char = (uint8_t*)(font8x8_basic[(uint8_t)c]);
    for (int row = 0; row < FONT_HEIGHT * scaleY; row++) {
        uint8_t pixels = font_char[(int)(row / scaleY)];
        for (int col = 0; col < FONT_WIDTH * scaleX; col++) {
            if (pixels & (1 << (int)(col / scaleX))) { // Check if the bit is set
                fb->fb[((y + row) * fb->pitch) + (x + col)] = colour.val;
            }
        }
    }
}

void draw_char(uint16_t x, uint16_t y, char c, VESA_Colour colour, float scaleX, float scaleY) {
    fb_draw_char(x, y, c, colour, scaleX, scaleY, vga_fb);
}

void fb_draw_string(uint16_t x, uint16_t y, const char* s, VESA_Colour colour, float scaleX, float scaleY, Framebuffer* fb) {
    while (*s) {
        fb_draw_char(x, y, *s, colour, scaleX, scaleY, fb);
        x += FONT_WIDTH * scaleX;
        s++;
    }
}

Framebuffer* allocate_framebuffer(uint32_t width, uint32_t height) {
    Framebuffer* out = kmalloc(sizeof(Framebuffer));
    if (out == NULL) {
        return NULL;
    }
    const uint32_t size = width * height * BYTES_PER_PIXEL;
    out->fb = kmalloc(size);
    if (out->fb == NULL) {
        return NULL;
    }
    out->width = width;
    out->height = height;
    out->pitch = width;
    return out;
}

Framebuffer* allocate_full_screen_framebuffer() {
    return allocate_framebuffer(Hres, Vres);
}

void deallocate_framebuffer(Framebuffer* fb) {
    kfree((void*)fb->fb);
    kfree((void*)fb);
}

void _blit(Framebuffer* srcptr, Framebuffer* destptr, int x, int y, uint32_t width, uint32_t height, int from_x, int from_y, int to_x, int to_y) {
    if (width < 0 || height < 0 || x > ((int)destptr->width) || y > ((int)destptr->height)) return;
    if (from_x > x + srcptr->width || from_x > destptr->width || from_y > y + srcptr->height || from_y > destptr->height) return;
    if (from_x < 0) from_x = 0;
    if (from_y < 0) from_y = 0;
    if (to_x > destptr->width) to_x = destptr->width;
    if (to_y > destptr->height) to_y = destptr->height;
    
    uint32_t src_offset = 0;

    if (y < 0) {
        // The following is negative because y is negative
        src_offset = -y * srcptr->pitch;
        height += y;
        y = 0;
    }

    if (x < 0) {
        // Again negative because x is negative
        src_offset -= x;
        width += x;
        x = 0;
    }

    uint32_t dest_offset = y * destptr->pitch + x;

    uint32_t xdiff = max(0, from_x - x);
    uint32_t ydiff = max(0, from_y - y);
    src_offset += xdiff;
    dest_offset += xdiff;
    src_offset += ydiff * srcptr->pitch;
    dest_offset += ydiff * destptr->pitch;
    width -= xdiff;
    height -= ydiff;

    width = min(width, to_x - from_x);
    height = min(height, to_y - from_y);

    width = min(width, destptr->width - x);
    height = min(height, destptr->height - y);

    width = min(width, srcptr->width);
    height = min(height, srcptr->height);

    if (srcptr->transparent){ // If transparency is enabled, blitting will be faster, as we have to check alpha for each byte. TODO: Improve performance
        for (uint32_t i = 0; i < height; i++) {
            for (uint32_t j = 0; j < width; j++) {
                if (((VESA_Colour)(srcptr->fb[src_offset + j])).a) {// For now, only 0x00 and 0xFF transparency values are supported
                    /* This is some test code to check if width and height are set properly. */
                    // if (from_x != 0){
                    //     destptr->fb[dest_offset + j] = (src_offset / srcptr->pitch) << 16 | (j) << 8 | 0xFF;
                    //     continue;
                    // }
                    destptr->fb[dest_offset + j] = srcptr->fb[src_offset + j];
                }
            }
            dest_offset += destptr->pitch;
            src_offset += srcptr->pitch;
        }
    } else {
        for (uint32_t i = 0; i < height; i++) {
            // memcpy(destptr->fb + dest_offset, srcptr->fb + src_offset, width);
            memcp(srcptr->fb + src_offset, destptr->fb + dest_offset, width * 4);
            // for (uint32_t j = 0; j < width; j++) {
            //     destptr->fb[dest_offset + j] = srcptr->fb[src_offset + j];
            // }
            dest_offset += destptr->pitch;
            src_offset += srcptr->pitch;
        }
    }
}

void blit(Framebuffer* src, Framebuffer* dest, int x, int y, uint32_t width, uint32_t height) {
    _blit(src, dest, x, y, width, height, 0, 0, dest->width, dest->height);
}

void blit_all_at(Framebuffer* src, Framebuffer* dest, int x, int y) {
    blit(src, dest, x, y, src->width, src->height);
}

void blit_all_at_only(Framebuffer* src, Framebuffer* dest, int x, int y, int from_x, int from_y, int to_x, int to_y) {
    _blit(src, dest, x, y, src->width, src->height, from_x, from_y, to_x, to_y);
}


void blit_clamped(Framebuffer* src, Framebuffer* dest, int from_x, int from_y, int to_x, int to_y) {
    Recti fb = recti_of_framebuffer(dest);

    int fx = max(from_x, fb.x);
    int fy = max(from_y, fb.y);
    int tx = min(to_x, fb.x + fb.width);
    int ty = min(to_y, fb.y + fb.height);

    if (fx < tx && fy < ty) { // To not call blit if we are outside the fb
        blit_all_at_only(src, dest, 0, 0, fx, fy, tx, ty);
    }
}

void blit_all_at_only_square(Framebuffer* src, Framebuffer* dest, int x, int y, Recti square, int width) {
    int half = width / 2;

    for (int i = -half; i <= half; ++i) {
        int left = square.x + i;
        int right = square.x + square.width + i;
        int top = square.y + i;
        int bottom = square.y + square.height + i;

        // Top
        blit_clamped(src, dest,
            square.x - half,
            top,
            square.x + square.width + half,
            top + 1);

        // Bottom
        blit_clamped(src, dest,
            square.x - half,
            bottom - 1,
            square.x + square.width + half,
            bottom);

        // Left
        blit_clamped(src, dest,
            left,
            square.y + half,
            left + 1,
            square.y + square.width - half);

        // Right
        blit_clamped(src, dest,
            right - 1,
            square.y - half,
            right,
            square.y + square.width - half);
    }
}


void fb_draw_gradient_at_only(int x, int y, int width, int height, VESA_Colour col1, VESA_Colour col2, Framebuffer* fb, Recti bounds) {
    int original_height = height;
    int start_y = max(y, bounds.pos.y);
    int start_x = max(x, bounds.pos.x);

    int end_x = min(x + width, bounds.x + bounds.width);
    int end_y = min(y + height, bounds.y + bounds.height);

    for (int i = start_y; i < end_y; i++) {
        VESA_Colour col = vesa_interpolate(col1, col2, (float)i / original_height);
        for (int j = start_x; j < end_x; j++) {
            fb->fb[(y + i) * fb->pitch + (x + j)] = col.val;
        }
    }
}

void fb_draw_gradient(int x, int y, int width, int height, VESA_Colour col1, VESA_Colour col2, Framebuffer* fb) {
    fb_draw_gradient_at_only(x, y, width, height, col1, col2, fb, recti_of_framebuffer(fb));
}

VESA_Colour vesa_interpolate(VESA_Colour col1, VESA_Colour col2, float t) {
    VESA_Colour out;
    out.r = col1.r + (col2.r - col1.r) * t;
    out.g = col1.g + (col2.g - col1.g) * t;
    out.b = col1.b + (col2.b - col1.b) * t;
    out.a = col1.a + (col2.a - col1.a) * t;
    return out;
}

Recti recti_of_framebuffer(Framebuffer* fb) {
    Recti r;
    r.x = 0;
    r.y = 0;
    r.width = fb->width;
    r.height = fb->height;
    return r;
}

#endif