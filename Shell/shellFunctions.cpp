#include "../Utils/Typedefs.h"
#include "../Drivers/VGA_Text.h"
#include "../Utils/Conversions.h"
#include "../Memory/mem.h"
#include "../Drivers/Floppy.h"
#include "../Utils/string.h"

#include "shellFunctions.h"

extern const char* currentTask;

extern uint16_t CursorPos;

static const char *  const helpList[5] = {                  // find better (dynamic) way
    "  help: shows command list\n",
    "  echo [text]: prints text\n",
    "  usedmem: shows dynamic memory usage\n",
    "  floppy:\n         list: shows list of connected floppy drives\n         []: shows floppy help\n",
    "  clear: clears the screen\n"
    
};

// Bind this to CMDs and use the help texts for each command (that's the better way ^)
void helpCMD(const char* s){
    if(strLen(s) == 0){
        currentTask = "help";
        kprint("List of commands:\n");
        for(int i = 0; i < sizeof(helpList)/sizeof(char*); i++)
            kprint(helpList[i]);
    }
    else{
        kprint("Invalid option: \"");
        if(s[0] == ' ') kprint((const char*)((int)s + 1));
        else kprint(s);
        kprint("\"");
    }
   
}

void printUsedMem(const char* s){
    if(strLen(s) == 0){
        currentTask = "usedmem";
        kprint("Used dynamic memory: ");
        kprint(toString(getFreeMem() - 0x10000, 10));
        kprint(" bytes");
    }
    else{
        kprint("Invalid option: \"");
        if(s[0] == ' ') kprint((const char*)((int)s + 1));
        else kprint(s);
        kprint("\"");
    }
}

void floppyCMD(const char* s){
    currentTask = "floppy";
    if(StringStartsWith(s, "list")) floppy_detect_drives();
    else if(false){;}       // will contain other floppy commands
    else{kprint(helpList[3]);}
}

void clearCMD(const char* s){ 
    if(strLen(s) == 0){
        currentTask = "clear";
        ClearScreen(-1);
        CursorPos = 0;
    }
    else{
        kprint("Invalid option: \"");
        if(s[0] == ' ') kprint((const char*)((int)s + 1));
        else kprint(s);
        kprint("\"");
    }
    
}

// Halt and catch fire, this is just a tester function for kpanic
void HCF(const char* s){
    // Try any exceptions you want
    int x = 0/0;
}

shellfunction shellf(void (*Fptr)(const char *), char* Alias, char* Help){
    shellfunction f;
    f.fptr = Fptr;
    kprint("<<");
    kprint(Alias);
    kprint(">>");
    f.alias = Alias;
    kprint("[[");
    kprint(f.alias);
    kprint("]]");
    f.help = Help;
    return f;
}

#define CMDENTRY(fptr, alias, help) {   \
    fptr,                               \
    alias,                              \
    help                                \
}

shellfunction CMDs[] = {
    CMDENTRY(&helpCMD, "help", "Shows command list"),
    CMDENTRY(&kprint, "echo", "Prints text"),
    CMDENTRY(&printUsedMem, "usedmem", "Shows dynamic memory usage"),
    CMDENTRY(&floppyCMD, "floppy", "Shows list of connected floppy drives"),
    CMDENTRY(&clearCMD, "clear", "Clears the screen"),
    CMDENTRY(&HCF, "hcf", "Crashes your system")
};

shellfunction* TryGetCMD(char* cmdbuf){
    for(int x = 0; x < sizeof(CMDs)/sizeof(shellfunction); x++){
        if(StringStartsWith(cmdbuf, CMDs[x].alias)){
            return &CMDs[x];
        }
    }
    return 0;
}
