#include "../Utils/Typedefs.h"
#include "../Drivers/VGA_Text.h"
#include "../Misc/colors.h"
#include "../Utils/string.h"
#include "shellFunctions.h"
#include "../Memory/mem.h"
#include "../Utils/Conversions.h"
/*

///////////////////////////////////// To add: command buffer, scroll navigation


FLOPPY:
    floppy list     [detect drives]
    

FILE SYSTEM: [...]

POWER MANAGEMENT: 
    shutdown
    reboot

MEMORY:
    usedmem
    
MISC:
    echo #string
    oldcmd
    
VARIABLES:
    let #name = #value
    


(+ load old modes)
*/

int CommandCursor;
extern int CursorPos;
extern int curMode;

extern int curColor;

const char* currentTask;

void load_shell(){
    curColor = DEFAULT_COLOR;
    currentTask = "init";
	ClearScreen(DEFAULT_COLOR);
	curMode = 10;
	ColLine(0, BAR_COLOR);
	ColLine(24, BAR_COLOR);
	ClrLine(24);
	SetCursorPos(0,0);
	kprint("MellOS shell - [");
    kprint("task: ");
    kprint(currentTask);
    kprint(" | ");
    kprint("dir: -");
    kprint("]\n");
    kprint("Work in progress, ");
    kprintCol("press F1 to use the old CMD mode.\n\n", ERROR_COLOR);
	CommandCursor = CursorPos;
	SetCursorPosRaw(1920);
	return;
    
}

void refreshShell(){
	SetCursorPos(0,0);
	kprint("MellOS shell - [");
    kprint("task: ");
    kprint(currentTask);
    kprint(" | ");
    kprint("dir: -");
    kprint("]\n");
	SetCursorPosRaw(1920);
	return;
}


char CommandBuffer[128];
void findCommand(){
    SetCursorPosRaw(CommandCursor);
    char* c = (char*)(0xB8000 + 2 * 1920);
	int i;
	for(i = 0; i < 2000 - CursorPos; i += 1)
		CommandBuffer[i] = *(c + 2 * i);
	CommandBuffer[i] = 0;
}


bool CheckCMD(void(*f)(const char*), const char* command){
    bool res = false;
    memrec();
    if(StringStartsWith(CommandBuffer, command)) {
        f(strDecapitate(CommandBuffer, strLen(command)));
        res = true;
    }
    memunrec();
    return res;
}

void parseCommand(){
    findCommand();
         if(CheckCMD(helpCMD, "help")){;}
    else if (CheckCMD(kprint, "echo ")){currentTask = "echo";}
    else if (CheckCMD(printUsedMem, "usedmem")){;}
    else if (CheckCMD(floppyCMD, "floppy ")){;}
    else if (CheckCMD(floppyCMD, "floppy")){;}      // get rid of redundancy asap
    else if (CheckCMD(clearCMD, "clear")){;}
    else if (CheckCMD(HCF, "hcf")){;}
    else {kprint("\""); kprint(CommandBuffer); kprint("\" is not a command");}
    kprint("\n");
    CommandCursor = CursorPos;
    ClrLine(24);
    //SetCursorPosRaw(1920);
    refreshShell();
}
