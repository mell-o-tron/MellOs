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
int curColor = DEFAULT_COLOR;
uint16_t CursorPos = 0; 		// Holds the current position of the cursor


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

void ClearScreen(int col){	//col...or
	if(col == -1) col = curColor;		//-1: maintain current colour
	switch(curMode){
	case 0:
		for(int i = (int)VIDEO_MEMORY; i < (int)VIDEO_MEMORY + 4000; i += 1) {
			if(i % 2 == 0) *((char*)i) = 32;	//Default is spaces
			else *((char*)i) = col;
		}
		break;
	default:
		
		for(int i = (int)VIDEO_MEMORY + 160; i < (int)VIDEO_MEMORY + 3840; i += 1) {
			if(i % 2 == 0) *((char*)i) = 32;	//Default is spaces
			else *((char*)i) = col;
		}
	}
}

void ColScreen(int col){
	switch(curMode){
	case 0:
		
		for(int i = (int)VIDEO_MEMORY + 1; i < (int)VIDEO_MEMORY + 4000; i += 2)
			*((char*)i) = col;
			break;
	default:
		for(int i = (int)VIDEO_MEMORY + 161; i < (int)VIDEO_MEMORY + 3840; i += 2) 
			*((char*)i) = col;
		
		
	}
}

					// SET LINE COLOUR
void ColLine(int line, int col){
	for(int i = (int)VIDEO_MEMORY + VGA_WIDTH * 2 * line + 1;
	i < (int)VIDEO_MEMORY + VGA_WIDTH * 2 * (line + 1) + 1; i += 2)  *((char*)i) = col;
		
}

void ClrLine(int line){
	for(int i = (int)VIDEO_MEMORY + VGA_WIDTH * 2 * line;
	i < (int)VIDEO_MEMORY + VGA_WIDTH * 2 * (line + 1); i += 2)  *((char*)i) = 0; 	// CLR WITH 0s
		
}

void scrollPageUp(){
    for(int i = 160*2; i < 4000 - 160; i++) *(VIDEO_MEMORY + i - 160) = *(VIDEO_MEMORY + i);
}


void kprint(const char* s){		// Just a simple print function. Prints to screen at cursor position, moves the cursor at the end. 
	uint8_t* charPtr = (uint8_t*)s;
	uint16_t i = CursorPos;
	while(*charPtr != 0){
	switch (*charPtr) {
		case 10:	if(i < 1920){
                        if(CursorPos >= 1760){
                            i = 1760;
                            scrollPageUp();
                        }else i+= VGA_WIDTH - i % VGA_WIDTH;	// ALSO ADDS RETURN TO NEWLINE!!
                    }
            
	  			
			break;
		case 13:
			i -= i % VGA_WIDTH;
			break;
		default:
        if(i < 1840){
            *(VIDEO_MEMORY + i * 2) = *charPtr;
            i++;
        }
        else{
            scrollPageUp();
            i = 1760;
        }
	}

	charPtr++;
	}
	SetCursorPosRaw(i);
	return;
}


void kprintCol(const char* s, int col){		//Print: with colours!
  uint8_t* charPtr = (uint8_t*)s;
  uint16_t i = CursorPos;
  while(*charPtr != 0)
  {
	switch (*charPtr) {
	  case 10:	
	  		if(CursorPos < 1920)
	  			i+= VGA_WIDTH - i % VGA_WIDTH;	// ALSO ADDS RETURN TO NEWLINE!!
			break;
	  case 13:
			i -= i % VGA_WIDTH;
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
	if(CursorPos >= 0 && CursorPos < 2000){
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
	return;
}

void MoveCursorUD(int i){			// MOVE CURSOR VERTICALLY
	if(CursorPos >= 0 && CursorPos < 2000){
		switch(curMode){
			case 0:
				CursorPos += VGA_WIDTH * i;
				SetCursorPosRaw(CursorPos);
				break;	
		}
	}
	return;
}

void ScrollVMem(){
	int i;
	
	for(i = 0xB8000/2 + 1999 ; i >= CursorPos + 0xB8000/2; i--)
		*((char*)(i * 2)) = *((char*)(i * 2 - 2));
	return;
}





void kprintChar(const char c, bool caps){
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
						CursorPos--;
						*(VIDEO_MEMORY + CursorPos * 2) = (char)0;
						break;
					default:
						if(CursorPos > 1920){
							CursorPos--;
							*(VIDEO_MEMORY + CursorPos * 2) = (char)0;
							}
				}
			}
			break;
		case 127: 						// del
			if(CursorPos < 2000){
			*(VIDEO_MEMORY + CursorPos * 2 + 2) = (char)0;
			}
			break;
		default:
			ScrollVMem();
			if(c >= 97 && c <= 172 && caps) *(VIDEO_MEMORY + CursorPos * 2) = c - 32; // CAPS
			else if(c >= 48 && c <= 57 && caps){
				//caps numbers
			}
			else *(VIDEO_MEMORY + CursorPos * 2) = c;
			
			if(CursorPos < 2000)
				CursorPos++;
            else scrollPageUp();
			
		}
	}
	
	SetCursorPosRaw(CursorPos);
	return;
}

void printError(const char* s){
    kprintCol(s, ERROR_COLOR);
}



