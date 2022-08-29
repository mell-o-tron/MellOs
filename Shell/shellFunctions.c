#include "../Utils/Typedefs.h"
#include "../Drivers/VGA_Text.h"
#include "../Utils/Conversions.h"
#include "../Memory/mem.h"
#include "../Drivers/Floppy.h"
#include "../Utils/string.h"

#include "shellFunctions.h"
#include "./functions/functions.h"

extern const char* currentTask;

extern uint16_t CursorPos;

static const char *  const helpList[5] = {                  // find better (dynamic) way
    "  help: shows command list\n",
    "  echo [text]: prints text\n",
    "  usedmem: shows dynamic memory usage\n",
    "  floppy:\n         list: shows list of connected floppy drives\n         []: shows floppy help\n",
    "  clear: clears the screen\n"
    "  meminfo: shows memory info\n"
    
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
    CMDENTRY(&helpCMD,  "help",     "Shows command list"),
    CMDENTRY(&echo,     "echo",     "Prints text"),
    CMDENTRY(&usedmem,  "usedmem",  "Shows dynamic memory usage"),
    CMDENTRY(&floppy,   "floppy",   "Shows list of connected floppy drives"),
    CMDENTRY(&clear,    "clear",    "Clears the screen"),
    CMDENTRY(&hcf,      "hcf",      "Crashes your system"),
    CMDENTRY(&meminfo,  "meminfo",  "Shows memory info")
};

shellfunction* TryGetCMD(char* cmdbuf){
    for(int x = 0; x < sizeof(CMDs)/sizeof(shellfunction); x++){
        if(StringStartsWith(cmdbuf, CMDs[x].alias)){
            return &CMDs[x];
        }
    }
    return 0;
}
