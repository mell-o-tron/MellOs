#include "../Utils/Typedefs.h"
#include "../Drivers/port_io.h"
#include "../Utils/Conversions.h"
#include "../Misc/colors.h"

/*********************
* TEXT MODE: 0xB8000 *
* GR.  MODE: 0xA000  *
*********************/

#define VIDEO_MEMORY		(char*)0xB8000
#define VGA_WIDTH	80


/*********************** FUNCTIONS ****************************
* SetCursorPosRaw / SetCursorPos: set cursor positon          *
* print: prints string                                        *
* other commented, currently red. or unnec. ones              *
* other useful functions handling colour and different modes  *
**************************************************************/

//thanks, OSDEV wiki, for being a thing.


//////////////////////////////////////////// WOW this code sucks, I'll need to rewrite it entirely with the new shell in mind.

extern int curMode;
extern char ker_tty[4000];
int curColor = DARK_COLOR;
uint16_t CursorPos = 0; 		// Holds the current position of the cursor

void display_tty(char* tty){
	for(int i = (int)VIDEO_MEMORY; i < (int)VIDEO_MEMORY + 4000; i += 1)
			*((char*)i) = tty[i - (int)VIDEO_MEMORY];
}

void display_tty_line(char* tty, int line){
	for(int i = (int)VIDEO_MEMORY + (line * 160); i < (int)VIDEO_MEMORY + ((line + 1) * 160); i += 1)
			*((char*)i) = tty[i - (int)VIDEO_MEMORY];
}

void SetCursorPosRaw(uint16_t pos){	// Does some I/O black magic 
	if(pos >= 0 && pos < 2000) {
		outb(0x3d4, 0x0f);
		outb(0x3d5, (uint8_t)(pos & 0xff));
		outb(0x3d4, 0x0e);
		outb(0x3d5, (uint8_t)((pos >> 8) & 0xff));
		CursorPos = pos;
	}
	return;
}


void SetCursorPos(int x, int y){
	uint16_t pos;

			pos = y * VGA_WIDTH + x;
			SetCursorPosRaw(pos);
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

void clear_tty(int col, char *tty){	//col...or
	if(col == -1) col = curColor;		//-1: maintain current colour
	for(int i = 0; i < 4000; i += 1) {
			if(i % 2 == 0) tty[i] = 32;	//Default is spaces
			else tty[i] = col;
	}

}


void col_tty(int col, char* tty){
		for(int i = 1; i < 4000; i += 2)
			tty[i] = col;
}

					// SET LINE COLOUR
void col_tty_line(int line, int col, char* tty){
	for(int i = VGA_WIDTH * 2 * line + 1;
	i < VGA_WIDTH * 2 * (line + 1) + 1; i += 2)  tty[i] = col;
}

void clr_tty_line(int line, char *tty){
	for(int i = VGA_WIDTH * 2 * line;
	i < VGA_WIDTH * 2 * (line + 1); i += 2)  tty[i] = 0; 	// CLR WITH 0s
		
}

void kprintChar(const char c, bool caps);


void kprint(const char* s){		// Just a simple print function; prints to screen at cursor position.
	uint8_t* charPtr = (uint8_t*)s;
	uint16_t i = CursorPos;
	while(*charPtr != 0){
	switch (*charPtr) {
		default:
        if(i < 1840){
			kprintChar(*charPtr, 0);
			i++;
        }
        else{
			clear_tty(curColor, ker_tty);
			display_tty(ker_tty);
			CursorPos = 80;
		}
	}

	charPtr++;
	}
	return;
}


void kprintCol(const char* s, int col){		//Print: with colours!
  uint8_t* charPtr = (uint8_t*)s;
  uint16_t i = CursorPos;
  while(*charPtr != 0)
  {
	switch (*charPtr) {
	  case '\n':
	  	i += 80;
		i -= i%80;
		break;
	  case 0x0d:
	  	break;
	  default:
	  *(VIDEO_MEMORY + i*2) = *charPtr;
	  *(VIDEO_MEMORY + i*2 + 1) = col;
	  i++;
	}

	charPtr++;
  }
  SetCursorPosRaw(i);
  return;
}


void MoveCursorLR(int i){			// MOVE CURSOR HORIZONTALLY
	if((CursorPos > 0 && i < 0) || (CursorPos < 1999 && i > 0)){
		switch(curMode){
			case 0:
				CursorPos += i;
				SetCursorPosRaw(CursorPos);
				break;
			default:
				if(!(i < 0 && CursorPos == 1920)){
					CursorPos += i;
					SetCursorPosRaw(CursorPos);
				}	
		}
	}
	else if (i < 0) {
		CursorPos = 0;
		SetCursorPosRaw(CursorPos);
	}
	else {
		CursorPos = 1999;
		SetCursorPosRaw(CursorPos);
	}
	return;
}

void MoveCursorUD(int i){			// MOVE CURSOR VERTICALLY

	if((CursorPos / 80 < 24 && i > 0) || (CursorPos / 80 > 0 && i < 0)){
		switch(curMode){
			case 0:
				CursorPos += VGA_WIDTH * i;
				SetCursorPosRaw(CursorPos);
				break;
		}
	}
	else if (i < 0) {
		CursorPos = 0;
		SetCursorPosRaw(CursorPos);
	}
	else {
		CursorPos = 1999;
		SetCursorPosRaw(CursorPos);
	}
	return;
}

void ScrollVMem(){
	int i;
	
	for(i = 0xB8000/2 + 1999 ; i >= CursorPos + 0xB8000/2; i--)
		*((char*)(i * 2)) = *((char*)(i * 2 - 2));
	return;
}



void print_tty_char(const char c, bool caps, char* tty){
	int curLine = 1 + CursorPos / VGA_WIDTH;

    
	if(c == 8 || c == 10 || (c >= 32 && c <= 255)) {
	switch(c){
		case 10:
			if(CursorPos < 1920){ 						// newline
                CursorPos+=VGA_WIDTH - CursorPos % VGA_WIDTH;
            }
			break;
		case 8: 						// backspace
			if(CursorPos > 0){
				switch(curMode){
					case 0:
						if (CursorPos > 0){
							CursorPos--;
							*(tty + CursorPos * 2) = (char)0;
						}
						break;
					default:
						if(CursorPos > 1920){
							CursorPos--;
							*(tty + CursorPos * 2) = (char)0;
						}
				}
			}
			break;
		case 127: 						// del
			if(CursorPos < 2000){
			*(tty + CursorPos * 2 + 2) = (char)0;
			}
			break;
		default:
			//ScrollVMem();
			if(c >= 97 && c <= 172 && caps) *(tty + CursorPos * 2) = c - 32; // CAPS
			else if(c >= 48 && c <= 57 && caps){
				//caps numbers
			}
			else *(tty + CursorPos * 2) = c;
			
			if(CursorPos < 1999)
				CursorPos++;
            else {
				SetCursorPosRaw(0);
				clear_tty(curColor, tty);
				display_tty(tty);
			}

		}
		display_tty_line(tty, curLine-1);
	}
	
	SetCursorPosRaw(CursorPos);
	return;
}


void kprintChar(const char c, bool caps){
	print_tty_char(c, caps, ker_tty);
}


void printError(const char* s){
    kprintCol(s, ERROR_COLOR);
}



