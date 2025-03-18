#ifdef VGA_VESA
#include "../utils/typedefs.h"
#include "../drivers/port_io.h"
#include "../utils/conversions.h"
#include "../utils/assert.h"
#include "../misc/colours.h"
// #include "../fonts/PSF.h"
// #define SSFN_CONSOLEBITMAP_TRUECOLOR        /* use the special renderer for 32 bit truecolor packed pixels */
// #define SSFN_NOIMPLEMENTATION
// #define SSFN_CONSOLEBITMAP_CLEARBG
// #include "../fonts/ssfn.h"
#include "vesa.h"

struct VbeInfoBlock {
	char signature[4];
	uint16_t version;
	uint32_t oem;
	uint32_t capabilities;
	uint32_t video_modes;
	uint16_t video_memory;
	uint16_t software_rev;
	uint32_t vendor;
	uint32_t product_name;
	uint32_t product_rev;
	uint8_t reserved[222];
	uint8_t oem_data[256];
} __attribute__((packed));

struct VBEModeInfoBlock {
	uint16_t attributes;
	uint8_t window_a;
	uint8_t window_b;
	uint16_t granularity;
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;
	uint16_t pitch;
	uint16_t width;
	uint16_t height;
	uint8_t w_char;
	uint8_t y_char;
	uint8_t planes;
	uint8_t bpp;
	uint8_t banks;
	uint8_t memory_model;
	uint8_t bank_size;
	uint8_t image_pages;
	uint8_t reserved0;
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
	uint32_t framebuffer;
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;
	uint8_t reserved1[206];
} __attribute__((packed));

struct VBEScreen {
	uint16_t mode;
	uint16_t width;
	uint16_t height;
	uint8_t bpp;
	uint16_t pitch;
	uint32_t framebuffer;
} __attribute__((packed));

// extern uint16_t vbe_width;
// extern uint16_t vbe_height;
// extern uint16_t vbe_bpp;
// extern uint16_t vbe_pitch;
// extern uint32_t vbe_framebuffer;
// extern uint16_t vbe_mode;
// extern uint16_t vbe_offset;
// extern uint16_t vbe_segment;

#define VBE_INFO_LOC		(char*)0x5300
#define VBE_MODE_INFO_LOC	(char*)0x5300 + sizeof(struct VbeInfoBlock)
#define VBE_SCREEN_LOC		(char*)0x5300 + sizeof(struct VbeInfoBlock) + sizeof(struct VBEModeInfoBlock)


#define FONT_WIDTH 8
#define FONT_HEIGHT 8
#define VSCALE 2
#define HSCALE 1
#define FONT_HOFFSET 0
#define FONT_VOFFSET 0
#define CONSOLE_HRES 640
#define CONSOLE_VRES 480
#define CONSOLE_HOFF 640
#define CONSOLE_VOFF 300
// #define CONSOLE_HRES 1920
// #define CONSOLE_VRES 1080
// #define CONSOLE_HOFF 0
// #define CONSOLE_VOFF 0
// TODO: Change CONSOLE_HRES/VRES to x.width/height once the framebuffer works correctly
#define CONSOLE_WIDTH(x) ((CONSOLE_HRES / (FONT_WIDTH + FONT_HOFFSET)) / HSCALE)
#define CONSOLE_HEIGHT(x) ((CONSOLE_VRES / (FONT_HEIGHT + FONT_VOFFSET)) / VSCALE)
#define HSLOT(x) ((cursor_pos % CONSOLE_WIDTH(x)) * (FONT_WIDTH + FONT_HOFFSET) * HSCALE)
#define VSLOT(x) ((cursor_pos / CONSOLE_WIDTH(x)) * (FONT_HEIGHT + FONT_VOFFSET) * VSCALE)

static struct VbeInfoBlock* vbe_info = (struct VbeInfoBlock*)VBE_INFO_LOC;
static struct VBEModeInfoBlock* vbe_mode_info = (struct VBEModeInfoBlock*)VBE_MODE_INFO_LOC;
static struct VBEScreen* vbe_screen = (struct VBEScreen*)VBE_SCREEN_LOC;

static uint16_t cursor_pos = 0;
static Framebuffer fb;

void _vesa_text_init(){
	fb = allocate_framebuffer(CONSOLE_HRES, CONSOLE_VRES);
	// fb = vga_fb;
	char buf[256];
	tostring(fb.fb, 16, buf);
	kprint(buf);
	// while(true);
}

void set_cursor_pos_raw(uint16_t pos){
	cursor_pos = pos;
}

uint16_t get_cursor_pos_raw(){
	return cursor_pos;
}

void increment_cursor_pos(){
	cursor_pos = (cursor_pos + 1) % ((CONSOLE_WIDTH(fb) - 1) * (CONSOLE_HEIGHT(fb) - 1));
}

void clear_line_col(uint32_t line, Colour col){}

void kclear_screen(){
	fb_clear_screen(fb);
	set_cursor_pos_raw(0);
}

void kprint_col(const char* s, Colour col){
	VESA_Colour fg = {0xFF, 0xFF, 0xFF, 0xFF};
	while (*s) {
		if (*s == '\n') {
			cursor_pos += CONSOLE_WIDTH(fb) - cursor_pos % CONSOLE_WIDTH(fb);
		} else {
			fb_draw_char(HSLOT(fb), VSLOT(fb), *s, fg, HSCALE, VSCALE, fb);
			// draw_char(HSLOT(fb), VSLOT(fb), *s, fg, HSCALE, VSCALE);
			increment_cursor_pos();
		}
		s++;
	}
	blit(fb, vga_fb, CONSOLE_HOFF, CONSOLE_VOFF, CONSOLE_HRES, CONSOLE_HRES);
}

void kprint(const char* s){
	kprint_col(s, 0x0F);
}

void kprint_char (char c, bool caps){
	c = c - (caps && c <= 122 && c >= 97 ? 32 : 0);

	VESA_Colour fg = {0xFF, 0, 0xFF, 0xFF};
	fb_draw_char(HSLOT(fb), VSLOT(fb), c, fg, HSCALE, VSCALE, fb);
	// draw_char(HSLOT(fb), VSLOT(fb), c, fg, HSCALE, VSCALE);
	blit(fb, vga_fb, CONSOLE_HOFF, CONSOLE_VOFF, CONSOLE_HRES, CONSOLE_HRES);
	increment_cursor_pos();
}

void move_cursor_LR(int i){			// MOVE CURSOR HORIZONTALLY
	if (i < 0){
		if (cursor_pos == 0) return;
		set_cursor_pos_raw(cursor_pos - 1);

	} else {
		if (cursor_pos == CONSOLE_WIDTH(fb) * CONSOLE_HEIGHT(fb) - 1) return;
		set_cursor_pos_raw(cursor_pos + 1);
	}
}

void move_cursor_UD(int i){			// MOVE CURSOR VERTICALLY
	if((cursor_pos / CONSOLE_WIDTH(fb) < (CONSOLE_HEIGHT(fb) - 1) && i > 0) || (cursor_pos / CONSOLE_WIDTH(fb) > 0 && i < 0)){
		cursor_pos += CONSOLE_WIDTH(fb) * i;
		set_cursor_pos_raw(cursor_pos);
	}
	else if (i < 0) {
		cursor_pos = 0;
		set_cursor_pos_raw(cursor_pos);
	}
	else {
		cursor_pos = (CONSOLE_WIDTH(fb) * CONSOLE_HEIGHT(fb) - 1);
		set_cursor_pos_raw(cursor_pos);
	}
	return;
}

void print_error(const char* s){
	if (!s) return;
    kprint_col(s, ERROR_COLOUR);
}
#endif