#include "../Utils/Typedefs.h"
#include "../Drivers/port_io.h"
#include "../Utils/Conversions.h"
#include "../Drivers/VGA_Text.h"
#include "../Utils/string.h"
#include <colors.h>

//very simple BF-ish interpreter without loops.

/********************FUNCTIONS*********************
* EnterCodeMode: sets up code mode, curMode = 2   *
* ClearMAr: clears "memory array"                 *
* Interpret: interprets program                   *
**************************************************/

char programBuffer[128];

int memArray[256];
int LocPointer;

extern int CursorPos;
extern int TextCursorPos;
extern int curMode;
void ClearMAr();


void EnterCodeMode(){
	ClearMAr();
	curMode = 0;
	ClearScreen(DEFAULT_COLOR);
	curMode = 2;
	ColLine(0, BAR_COLOR);
	ColLine(24, BAR_COLOR);
	ClrLine(24);
	SetCursorPos(0,0);
	kprint("CODE mode\n");
	
	TextCursorPos = CursorPos;
	SetCursorPosRaw(1920);
	
}


void ClearMAr(){
	for(int i = 0; i < 70; i++) memArray[i] = 0;
}


void Interpret(){
	char* c = (char*)(0xB8000 + 2 * 1920);		//Saves program to buffer
	for(int i = 0; i < 2000 - CursorPos; i += 1)
		programBuffer[i] = *(c + 2 * i);
		
	ClearMAr();
	ClearScreen(DEFAULT_COLOR);
	SetCursorPosRaw(81);
	int i = 0;
	while(programBuffer[i] != 0 || i >= 80){
		switch(programBuffer[i]){
			case '>': LocPointer == 255? LocPointer = 0 : LocPointer++; break;
			case '<': LocPointer == 0? LocPointer = 255 : LocPointer--; break;
			case '+': memArray[LocPointer]++; break;
			case '-': if(LocPointer>0)memArray[LocPointer]--; break;
			case '.': kprint("output: "); kprint(toString(memArray[LocPointer], 10)); kprint("    "); break; 
		}
		i++;
	}
	ClrLine(24);
	SetCursorPosRaw(1920);

}
