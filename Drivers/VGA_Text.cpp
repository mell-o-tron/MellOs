#pragma once
#include "port_io.cpp"

/*********************
* TEXT MODE: 0xB8000 *
* GR.  MODE: 0xA000  *
*********************/

#define VRAM		(char*)0xB8000
#define VGA_WIDTH	80

//thanks, OSDEV wiki, for being a thing.


void SetCursorPosRaw(uint16_t pos){
	outb(0x3d4, 0x0f);
	outb(0x3d5, (uint8_t)(pos & 0xff));
	outb(0x3d4, 0x0e);
	outb(0x3d5, (uint8_t)((pos >> 8) & 0xff));
	return;
}

uint16_t CursorPos = 0;
void SetCursorPos(int x, int y){
	uint16_t pos = y * VGA_WIDTH + x;
	SetCursorPosRaw(pos);
	CursorPos = pos;
	return;
}

/*uint16_t GetCursorPos()		//Commented for no particular reason, uncomment if needed
{
    uint16_t pos = 0;
    outb(0x3D4, 0x0F);
    pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= ((uint16_t)inb(0x3D5)) << 8;
    return pos;
}

void disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
 
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}
*/


void print(const char* s){
  uint8_t* charPtr = (uint8_t*)s;
  uint16_t i = CursorPos;
  while(*charPtr != 0)
  {
    switch (*charPtr) {
      case 10:	
      		i+= VGA_WIDTH;
        	break;
      case 13:
        	i -= i % VGA_WIDTH;
        	break;
      default:
      *(VRAM + i * 2) = *charPtr;
      i++;
    }

    charPtr++;
  }
  SetCursorPosRaw(i);
}





