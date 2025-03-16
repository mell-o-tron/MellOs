#define VGA_VESA
#ifdef VGA_VESA
#include "../utils/typedefs.h"
#include "../drivers/port_io.h"
#include "../utils/conversions.h"
#include "../utils/assert.h"
#include "../misc/colours.h"

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

#define VIDEO_MEMORY		(char*)0xB8000
#define VGA_WIDTH			80
#define VGA_HEIGHT  		25
#define BYTES_PER_CHAR		2

#define VBE_INFO_LOC		(char*)0x5300
#define VBE_MODE_INFO_LOC	(char*)0x5300 + sizeof(struct VbeInfoBlock)
#define VBE_SCREEN_LOC		(char*)0x5300 + sizeof(struct VbeInfoBlock) + sizeof(struct VBEModeInfoBlock)

#define FRAMEBUFFER			((volatile uint32_t*)0x800000)

static struct VbeInfoBlock* vbe_info = (struct VbeInfoBlock*)VBE_INFO_LOC;
static struct VBEModeInfoBlock* vbe_mode_info = (struct VBEModeInfoBlock*)VBE_MODE_INFO_LOC;
static struct VBEScreen* vbe_screen = (struct VBEScreen*)VBE_SCREEN_LOC;
static volatile uint32_t *framebuffer;
static uint8_t bytes_per_pixel;

void _vesa_framebuffer_init(void){
	// framebuffer = (uint8_t*)vbe_screen->framebuffer;
	framebuffer = (volatile uint32_t*)0x800000;
	// framebuffer = (uint32_t*)(vbe_info->signature);
	bytes_per_pixel = vbe_screen->bpp / 8;
	clear_screen_col(BLUE_COLOUR);
}

void set_cursor_pos_raw(uint16_t pos){}
uint16_t get_cursor_pos_raw(){return 0;}

void clear_screen_col (Colour col) {
	// for(size_t i = 0; i < 1920; i++) {
	// 	memset(framebuffer + (1920) * i, 0xFF00FF00, (1080) * bytes_per_pixel);
	// }
	// for (size_t y = 0; y < vbe_screen->height; y++) {
	// 	for (size_t x = 0; x < vbe_screen->width; x++) {
	// 		size_t offset = (y * vbe_screen->pitch) + (x * bytes_per_pixel);
	// 		for (size_t b = 0; b < bytes_per_pixel; b++) {
	// 			((uint32_t*)0x800000)[offset + b] = 0xFF << b;
	// 			kprint("clearing");
	// 		}
	// 	}
	// }
	VESA_Colour vesa_col;
	vesa_col.r = 255;
	vesa_col.g = 0;
	vesa_col.b = 0;
	vesa_col.a = 0xFF;
	vesa_col = vga2vesa(col);
	for (size_t y = 0; y < 1920 * 1080; y++) {
		FRAMEBUFFER[y] = vesa_col.val;
	}
	// char buf[512];
	// // framebuffer = 0x42424242;
	// tostring(vbe_info->signature, 10, buf);
	// kprint(buf);
	// kprint(tostring(vbe_mode_info->YResolution, 10));
	// while(true);
}


void clear_line_col(uint32_t line, Colour col){}
void kprint_col(const char* s, Colour col){}
void kprint(const char* s){}
void kprint_char (char c, bool caps){
	
}

void move_cursor_LR(int i){}
void move_cursor_UD(int i){}

void print_error(const char* s){}
#endif