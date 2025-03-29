#pragma once

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long size_t;
typedef uint8_t bool;
#define true    1
#define false   0

/**************** SEND/RECEIVE BYTE ******************/
void outb(uint16_t port, uint8_t data){
	asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
	return;
}

uint8_t inb(uint16_t port){
	uint8_t res;
	asm volatile("inb %1, %0" : "=a"(res) : "Nd"(port));
	return res;
}

/**************** SEND/RECEIVE WORD ******************/

void outw(uint16_t port, uint16_t value)
{
	asm volatile ("outw %w0, %1" : : "a" (value), "id" (port) );
}

uint16_t inw(uint16_t port){
   uint16_t ret;
   asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
} 

/**************** SEND/RECEIVE LONG (32-BIT) ******************/

void outl(uint16_t port, uint32_t value){
	asm volatile ("outl %%eax, %%dx" :: "d" (port), "a" (value));
}

uint32_t inl(uint16_t port){
   uint32_t ret;
   asm volatile ("inl %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
} 



#define VIDEO_MEMORY		(char*)0xB8000
#define VGA_WIDTH			80
#define VGA_HEIGHT  		25
#define BYTES_PER_CHAR		2
uint8_t VGA_DUMMY __attribute__((section(".vga_fb")));

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

void kprint(const char* s){	
	if (!s) return;
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
				// scroll_up(1);
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
		i++;
		}

		char_ptr++;
	}
	set_cursor_pos_raw(i);
  return;
}



uint32_t strlen(const char* s){
	uint32_t res;
	for(res = 0; s[res] != 0; res++);
	return res;
}

void reverse(char s[])
{
    uint32_t length = strlen(s) ;
    uint32_t c, i, j;

    for (i = 0, j = length - 1; i < j; i++, j--){
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

const char* tostring(int n, int base, char* dest) {
    
	char* buffer = dest;
	
	int m = n;
	int i = 0;
    
    if(n < 0) m = -n;
	
    while(m != 0){
		buffer[i] = (char)((m % base)+ (m % base > 9 ? 55 : 48));
		m = m / base;
		i++;
	}
    
    if(n < 0){
        buffer[i] = '-';
        i++;
    }
    
    if(n == 0){
        buffer[i] = '0';
        i++;
    }
	
	buffer[i] = '\0';

	reverse(buffer);

	return buffer;
}