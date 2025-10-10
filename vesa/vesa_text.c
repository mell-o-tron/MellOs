#ifdef VGA_VESA
#include "vesa_text.h"
#include "stddef.h"
#include "conversions.h"
#include "colours.h"
#include "include/vesa.h"
#include "dynamic_mem.h"
#include "stdbool.h"

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

#define VBE_INFO_LOC		(char*)0x5300
#define VBE_MODE_INFO_LOC	((char*)0x5300 + sizeof(struct VbeInfoBlock))
#define VBE_SCREEN_LOC		((char*)0x5300 + sizeof(struct VbeInfoBlock) + sizeof(struct VBEModeInfoBlock))


#define FONT_WIDTH 8
#define FONT_HEIGHT 8
#define VSCALE 2
#define HSCALE 1
#define FONT_HOFFSET 1
#define FONT_VOFFSET 1
#define CONSOLE_HRES (Hres > 1000 ? 640 : (Hres - 10))
#define CONSOLE_VRES (Vres > 700  ? 480 : (Vres - 30))
#define CONSOLE_HOFF ((Hres - CONSOLE_HRES) / 2)
#define CONSOLE_VOFF ((Vres - CONSOLE_VRES) / 2)
// #define CONSOLE_HRES 1920
// #define CONSOLE_VRES 1080
// #define CONSOLE_HOFF 0
// #define CONSOLE_VOFF 0
#define CONSOLE_WIDTH(x) ((x->width / (FONT_WIDTH + FONT_HOFFSET)) / HSCALE)
#define CONSOLE_HEIGHT(x) ((x->height / (FONT_HEIGHT + FONT_VOFFSET)) / VSCALE)
#define HSLOT(x) (cursor_pos % CONSOLE_WIDTH(x))
#define VSLOT(x) (cursor_pos / CONSOLE_WIDTH(x))
#define CHAR_HEIGHT ((FONT_HEIGHT + FONT_VOFFSET) * VSCALE)
#define CHAR_WIDTH ((FONT_WIDTH + FONT_HOFFSET) * HSCALE)
#define HPOS(x) (HSLOT(x) * CHAR_WIDTH)
#define VPOS(x) (VSLOT(x) * CHAR_HEIGHT)

static struct VbeInfoBlock* vbe_info = (struct VbeInfoBlock*)VBE_INFO_LOC;
static struct VBEModeInfoBlock* vbe_mode_info = (struct VBEModeInfoBlock*)VBE_MODE_INFO_LOC;
static struct VBEScreen* vbe_screen = (struct VBEScreen*)VBE_SCREEN_LOC;

static uint16_t cursor_pos = 0;
static Framebuffer* fb = NULL;
static bool autoblit = true;

static function_type dirty_callback = NULL;

void _vesa_text_set_dirty_callback(function_type f){
	dirty_callback = f;
}

void _vesa_text_init(){
	fb = allocate_framebuffer(CONSOLE_HRES, CONSOLE_VRES);
	fb->fb = kmalloc(fb->width * fb->height * BYTES_PER_PIXEL);
	// char buf[256];
	// tostring((int)fb->fb, 16, buf);
	// kprint(buf);
}

void _vesa_text_set_framebuffer(Framebuffer* f){
	fb = f;
}

Framebuffer* _vesa_text_get_framebuffer() {
	return fb;
}

void _vesa_text_set_autoblit(bool enabled){
	autoblit = enabled;
}

void set_cursor_pos_raw(uint16_t pos){
	cursor_pos = pos;
}

uint16_t get_cursor_pos_raw(){
	return cursor_pos;
}

void increment_cursor_pos(){
	cursor_pos = (cursor_pos + 1) % ((CONSOLE_WIDTH(fb)) * (CONSOLE_HEIGHT(fb)));
}

void clear_line_col(uint32_t line, Colour col){
	fb_fill_rect(0, line * CHAR_HEIGHT, fb->width, CHAR_HEIGHT, vga2vesa(col), *fb);
}

void scroll_up(){
	// Scroll by inplace blit to itself
	blit(*fb, *fb, 0, -CHAR_HEIGHT, fb->width, fb->height - CHAR_HEIGHT);
	clear_line_col(CONSOLE_HEIGHT(fb) - 2, DEFAULT_COLOUR);
	set_cursor_pos_raw(cursor_pos - CONSOLE_WIDTH(fb));
}

void kclear_screen(){
	// return;
	fb_clear_screen(fb);
	set_cursor_pos_raw(0);
}

void kprint_col(const char* s, Colour col){
	const VESA_Colour fg = {0xFF, 0xFF, 0xFF, 0xFF};
	while (*s) {
		if (*s == '\n') {
			cursor_pos += CONSOLE_WIDTH(fb) - cursor_pos % CONSOLE_WIDTH(fb);
			if (VSLOT(fb) >= CONSOLE_HEIGHT(fb) - 1) {
				scroll_up();
			}
		} else {
			size_t hpos = HSLOT(fb);
			fb_draw_char(HPOS(fb), VPOS(fb), *s, fg, HSCALE, VSCALE, *fb);
			increment_cursor_pos();
			if (hpos > HSLOT(fb)) { // TODO: Doesn't work on the last line for some reason. Fix
				cursor_pos += CONSOLE_WIDTH(fb) - hpos;
				if (VSLOT(fb) >= CONSOLE_HEIGHT(fb) - 1) {
					scroll_up();
				}
			}
		}
		s++;
	}

	if (autoblit) {
		blit(*fb, *vga_fb, CONSOLE_HOFF, CONSOLE_VOFF, fb->width, fb->height);
	}
	
	if (dirty_callback) {
		dirty_callback();
	}
}

void kprint(const char* s){
	kprint_col(s, 0x0F);
}

void kprint_char (char c, bool caps){
	c = c - (caps && c <= 122 && c >= 97 ? 32 : 0);

	VESA_Colour fg = {0xFF, 0xFF, 0xFF, 0xFF};
	// Blank out the slot for the next character. Needed to implement backspace as going back and printing a space
	fb_fill_rect(HPOS(fb), VPOS(fb), CHAR_WIDTH, CHAR_HEIGHT, vga2vesa(0x00), *fb);
	fb_draw_char(HPOS(fb), VPOS(fb), c, fg, HSCALE, VSCALE, *fb);
	if (autoblit){
		blit(*fb, *vga_fb, CONSOLE_HOFF, CONSOLE_VOFF, fb->width, fb->height);
	}
	
	if (dirty_callback) {
		dirty_callback();
	}
	
	increment_cursor_pos();
}

void kprint_dec(uint32_t n){
	char buf[11];
	tostring(n, 10, buf);
	kprint(buf);
	kprint("\n");
}

void kprint_hex(uint32_t n){
	char buf[11];
	tostring_unsigned(n, 16, buf);
	kprint(buf);
	kprint("\n");
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