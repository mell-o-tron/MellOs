#include "../utils/typedefs.h"
#include "../drivers/vga_text.h"
#include "../utils/conversions.h"
#include "../memory/mem.h"
#include "../utils/string.h"
#include "../shell/shell.h"
#include "../cpu/timer/timer.h"

#include "shell_functions.h"
#include "./functions/functions.h"

extern uint16_t CursorPos;

#define CMDENTRY(fptr, alias, help, usage) {    \
    fptr,                                       \
    alias,                                      \
    help,                                       \
    usage                                       \
}

void helpCMD(const char* s);

shellfunction CMDs[] = {
    CMDENTRY(&helpCMD,  "help",     "Shows command list", "help"),
    CMDENTRY(&echo,     "echo",     "Prints text", "echo [text]"),
    CMDENTRY(&clear,    "clear",    "Clears the screen", "clear"),
    CMDENTRY(&hcf,      "hcf",      "Crashes your system", "hcf"),
    CMDENTRY(&diskinfo,  "diskinfo",  "Shows disk info", "diskinfo"),
    CMDENTRY(&diskread,  "diskread",  "Reads from disk", "diskread [LBA] [num_sectors]"),
    CMDENTRY(&ls, "ls", "Lists files", "ls"),
    CMDENTRY(&newfile, "newfile", "Creates new file", "newfile [name]"),
    CMDENTRY(&write_file, "writefile", "Writes string to file", "writefile [filename] [string]"),
    CMDENTRY(&read_file, "readfile", "Reads string from file", "readfile [filename]"),
    CMDENTRY(&texted, "texted", "Opens text editor", "texted [filename]")
};


void helpCMD(const char* s){
    if(strlen(s) == 0){
        kprint("List of commands:\n");
        for(int i = 0; i < sizeof(CMDs)/sizeof(shellfunction); i++) {
            kprint("   ");
            kprint(CMDs[i].usage);
            kprint(": ");
            kprint(CMDs[i].help);
            kprint("\n");
        }
    }
    else{
        kprint("Invalid option: \"");
        if(s[0] == ' ') kprint((const char*)(s + 1));
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

shellfunction* TryGetCMD(char* cmdbuf){
    for(int x = 0; x < sizeof(CMDs)/sizeof(shellfunction); x++){
        if(string_starts_with(cmdbuf, CMDs[x].alias)){
            return &CMDs[x];
        }
    }
    return 0;
}
