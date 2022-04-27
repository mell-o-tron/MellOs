#include "../Utils/Typedefs.h"
#include "../Drivers/port_io.h"
#include "../Utils/Conversions.h"
#include "../Drivers/VGA_Text.h"
#include "../Utils/string.h"
#include "../CPU/Timer/timer.h"
#include "../Misc/CodeMode.h"
#include <colors.h>
/********************FUNCTIONS*********************
* SetCmdMode: sets up the CMD mode, curMode = 1   *
* FindCmd: reads command line, seeks commands     *
* ExecCmd: executes commands, based on cmdNum     *
* TextMode: placeholder for future text editor    *
**************************************************/





extern int curColor;

int cmdNum = 8;			// number of commands
const char* commands[] = {		// command labels
"",
"text",
"calc",
"code",
"help",
"BDR",
"darkmode",
"lightmode",
};


extern const char Fool[];
extern const char bdr[];


extern int curMode;			// current mode

extern int CursorPos;			

int TextCursorPos;			// cursor position to return to to after executing a command

void GetCmd();

void ExecCmd(int cmd, char* str);

void SetCmdMode(){
	curMode = 0;
	ClearScreen(DEFAULT_COLOR);
	curMode = 1;
	ColLine(0, BAR_COLOR);
	ColLine(24, BAR_COLOR);
	ClrLine(24);
	SetCursorPos(0,0);
	kprint("CMD mode\n");
	kprint(Fool);
    kprintCol("May 23 2021: CMD mode is being replaced by a more complete shell \nPress F5 to enter shell mode\n", ERROR_COLOR);
	TextCursorPos = CursorPos;
	SetCursorPosRaw(1920);
	return;	
}


char CmdBuffer[128];
void FindCmd(){			// uses StringsEqu function to compare the string in the command line and the labels.
					// should implement some "starts with" function or even some more complex string parsing
					// (regular expressions maybe?) for complex commands like "echo" or conditionals.
					// (we could make a programming language out of this -_-)
	char* c = (char*)(0xB8000 + 2 * 1920);
	int i;
	for(i = 0; i < 2000 - CursorPos; i += 1)
		CmdBuffer[i] = *(c + 2 * i);
	
	CmdBuffer[i] = 0;
	
	SetCursorPosRaw(TextCursorPos);
	
	bool found = false;
	for(int j = 0; j < cmdNum; j++){
		if(StringsEqu(CmdBuffer, commands[j])){
			found = true;
			ExecCmd(j, CmdBuffer);
			break;
		}
	}
	if(!found) ExecCmd(0, CmdBuffer);
	
	TextCursorPos = CursorPos; 
	ClrLine(24);
	if(curMode == 1) SetCursorPosRaw(1920);
	return;
}

void TextMode(){			// DUMMY FUNCTION (will eventually be useful)
	curMode = 0;
	ClearScreen(DARK_COLOR);
	SetCursorPosRaw(0);
	kprint("This is some text you can edit.\nActually this is pointless, press F1 to get back to cmd mode.");
	//there's a few bugs, as well!
}

void ExecCmd(int cmd, char* str){
	//kprint(toString(cmd, 10));
	
	switch(cmd){
		case 0: kprint("command not found \n"); break;
		case 1: TextMode(); break;
		case 3: EnterCodeMode(); break;
		case 4: kprint("Command list: ");
			for(int i = 0; i < cmdNum; i++){
				kprint(commands[i]);
				kprint("    ");
			}
			kprint("\n");
			break;
		case 5: 
			ClearScreen(0x0D);
			SetCursorPosRaw(81);
			kprint(bdr);
			kprintCol("          \"Addio, Bocca di Rosa, con te se ne parte la primavera\".              ", 0xe0);
			
			
			SetCursorPosRaw(81);
			asm volatile ("1: jmp 1b");
		case 6: ColScreen(DARK_COLOR); curColor = DARK_COLOR; break;
		case 7: ColScreen(DEFAULT_COLOR); curColor = DEFAULT_COLOR; break;
		default: kprint("not yet ");
	
	}

}
