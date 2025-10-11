#ifndef VGA_VESA
#include "vga_text.h"
#include "../utils/typedefs.h"
#include "../drivers/port_io.h"
#include "../utils/conversions.h"
#include "../utils/assert.h"
#include "../misc/colours.h"
#include "../memory/mem.h"
#include "drivers/uart.h"

/*********************
* TEXT MODE: 0xB8000 *
* GR.  MODE: 0xA000  *
*********************/

#define VIDEO_MEMORY		(char*)0xB8000
#define VGA_WIDTH			80
#define VGA_HEIGHT  		25
#define BYTES_PER_CHAR		2



void set_cursor_pos_raw(uint16_t pos){	// Does some I/O black magic
	if(pos >= 0 && pos < VGA_WIDTH * VGA_HEIGHT) {
		outb(0x3d4, 0x0f);
		outb(0x3d5, (uint8_t)(pos & 0xff));
		outb(0x3d4, 0x0e);
		outb(0x3d5, (uint8_t)((pos >> 8) & 0xff));
	}
	return;
}

uint16_t get_cursor_pos_raw()
{
	uint16_t pos = 0;
	outb(0x3D4, 0x0F);
	pos |= inb(0x3D5);
	outb(0x3D4, 0x0E);
	pos |= ((uint16_t)inb(0x3D5)) << 8;
	return pos;
}


void clear_screen_col (Colour col){
	for (uint32_t i = 0; i < 2 * (VGA_HEIGHT * VGA_WIDTH); i += 2){
		*(VIDEO_MEMORY + i) = ' ';
		*(VIDEO_MEMORY + i + 1) = col;
	}
}

void clear_line_col(uint32_t line, Colour col){
	for (uint32_t i = 2 * line * VGA_WIDTH; i < 2 * ((line+1) * VGA_WIDTH); i += 2){
		*(VIDEO_MEMORY + i) = ' ';
		*(VIDEO_MEMORY + i + 1) = col;
	}
}

void scroll_up(size_t lines){ // Copying memory from VGA to VGA is not the most efficient way to scroll (and relies on memcp being linear), but it's the easiest
	memcp(VIDEO_MEMORY + BYTES_PER_CHAR * VGA_WIDTH * lines, VIDEO_MEMORY, VGA_WIDTH * BYTES_PER_CHAR * (VGA_HEIGHT - lines));
	for (size_t i = VGA_HEIGHT - lines; i < VGA_HEIGHT; i++){
		clear_line_col(i, DEFAULT_COLOUR);
	}
}

void kprint_col(const char* s, Colour col){		//Print: with colours!
	if (!s) return;
    uart_print_all(s);
	uint8_t* char_ptr = (uint8_t*)s;

	uint16_t cursor_pos = get_cursor_pos_raw();
	uint16_t i = cursor_pos;
	while(*char_ptr != 0)
	{
		if (i > 2000) {
			i = 0;
		}

		switch (*char_ptr) {
		case '\n':
			if ((i / VGA_WIDTH) == VGA_HEIGHT - 1){
				scroll_up(1);
			} else {
				i += VGA_WIDTH;
			}
			i -= i%VGA_WIDTH;

			if (i > 2000)
				i = 0;

			break;
		case 0x0d:
			break;
		default:
		*(VIDEO_MEMORY + i*2) = *char_ptr;
		*(VIDEO_MEMORY + i*2 + 1) = col;
		i++;
		}

		char_ptr++;
	}
	set_cursor_pos_raw(i);
  return;
}

void kprint(const char* s){
	kprint_col(s, DEFAULT_COLOUR);
}

void kprint_char (char c, bool caps) {
	char s [2];
	s[0] = c - (caps && c <= 122 && c >= 97 ? 32 : 0);
	s[1] = 0;

	kprint_col(s, DEFAULT_COLOUR);
}


void move_cursor_LR(int i){			// MOVE CURSOR HORIZONTALLY
	uint16_t cursor_pos = get_cursor_pos_raw();

	if (i < 0){
		if (cursor_pos == 0) return;
		set_cursor_pos_raw(cursor_pos - 1);

	} else {
		if (cursor_pos == VGA_WIDTH * VGA_HEIGHT - 1) return;
		set_cursor_pos_raw(cursor_pos + 1);
	}
}

void move_cursor_UD(int i){			// MOVE CURSOR VERTICALLY
	uint16_t cursor_pos = get_cursor_pos_raw();
	if((cursor_pos / VGA_WIDTH < (VGA_HEIGHT - 1) && i > 0) || (cursor_pos / VGA_WIDTH > 0 && i < 0)){
		cursor_pos += VGA_WIDTH * i;
		set_cursor_pos_raw(cursor_pos);
	}
	else if (i < 0) {
		cursor_pos = 0;
		set_cursor_pos_raw(cursor_pos);
	}
	else {
		cursor_pos = (VGA_WIDTH * VGA_HEIGHT - 1);
		set_cursor_pos_raw(cursor_pos);
	}
	return;
}

void print_error(const char* s){
	if (!s) return;
    kprint_col(s, ERROR_COLOUR);
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
#endif