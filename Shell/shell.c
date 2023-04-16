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
    meminfo
    
MISC:
    echo #string
    
VARIABLES:
    let #name = #value
    

*/

extern char ker_tty[4000];

int CommandCursor;
extern int CursorPos;
extern int curMode;

extern int curColor;

const char* currentTask;

void load_shell(){
    curColor = DARK_COLOR;
    currentTask = "init";
	clear_tty(DARK_COLOR, ker_tty);
    clr_tty_line(24, ker_tty);
    display_tty(ker_tty);
	curMode = 10;
	SetCursorPos(0,0);
	kprint("MellOS shell - [");
    kprint("task: ");
    kprint(currentTask);
    kprint(" | ");
    kprint("dir: -");
    kprint("]\n");
    kprint("Work in progress;\n");
    kprintCol("Version note: the old \"mode\" system has been removed.\n\n", ERROR_COLOR);
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
    char* c = ker_tty;
	int i = 0;
    int j = 0;
    for (int i = 1920 * 2; i < 4000; i += 2){
        CommandBuffer[j] = c[i];
        j++;
    }
	CommandBuffer[j] = 0;
    
}

void parseCommand(){
    findCommand();
    memrec();
    if(CommandBuffer[0] == 0){;}
    else{
        shellfunction* cmd = TryGetCMD(CommandBuffer);
        if(cmd != 0){
            cmd->fptr(strDecapitate(CommandBuffer, strLen(cmd->alias)));
        }
        else {kprint("\""); kprint(CommandBuffer); kprint("\" is not a command");}
    }
    memunrec();
    kprint("\n");
    CommandCursor = CursorPos;
    clr_tty_line(24, ker_tty);
    display_tty(ker_tty);
    //SetCursorPosRaw(1920);
    refreshShell();
    
}
