#include "utils/typedefs.h"
#ifdef VGA_VESA
#include "drivers/vesa/vesa_text.h"
#else
#include "drivers/vga_text.h"
#endif
#include "utils/conversions.h"
#include "memory/mem.h"
#include "utils/string.h"
#include "shell/shell.h"
#include "cpu/timer/timer.h"

#include "shell_functions.h"
#include "shell/functions/functions.h"

#include "drivers/rtc.h"

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
    CMDENTRY(&rmfile, "rmfile", "Removes file", "rmfile [name]"),
    CMDENTRY(&write_file, "writefile", "Writes string to file", "writefile [filename] [string]"),
    CMDENTRY(&read_file, "readfile", "Reads string from file", "readfile [filename]"),
    CMDENTRY(&texted, "texted", "Opens text editor", "texted [filename]"),
    CMDENTRY(&exec, "exec", "Runs an executable", "exec [filename]"),
    CMDENTRY(&kill, "kill", "Kills a process", "kill [PID]"),
    CMDENTRY(&ps, "ps", "Lists running processes", "ps"),
    CMDENTRY(&erase_files, "erasefiles", "Erases the file bitmap", "erasefiles"),
    CMDENTRY(&kbSetLayout, "kblayout", "Selects a keyboard layout", "kblayout [name]"),
    #ifdef VGA_VESA
    CMDENTRY(&vell, "vell", "VESA graphic shell", "vell"),
    CMDENTRY(&frogues, "frogues", "Frog viewing program", "frogues"),
    CMDENTRY(&jb_juggle_balls, "jb", "Ball juggling program", "juggle_balls"),
    CMDENTRY(&sperkaster, "sp", "Perk raycasting engine", "sperkaster"),
    #endif
    CMDENTRY(&displayinfo, "displayinfo", "Display display info", "displayinfo"),
    CMDENTRY(&meminfo, "meminfo", "Displays memory info", "meminfo"),
    CMDENTRY(&print_time, "time", "Displays current time", "time"),
    #ifdef AUDIO_ENABLED
    CMDENTRY(&roll, "roll", "Get rolled", "roll"),
    #endif
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
