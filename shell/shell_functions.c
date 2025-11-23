#include "autoconf.h"
#include "stdint.h"
#ifdef CONFIG_GFX_VESA
#include "vesa_text.h"
#else
#include "vga_text.h"
#endif
#include "conversions.h"
#include "mem.h"
#include "shell/shell.h"
#include "string.h"
#include "timer.h"

#include "functions/functions.h"
#include "shell/shell_functions.h"

extern uint16_t CursorPos;

#define CMDENTRY(fptr, alias, help, usage) {fptr, alias, help, usage}

void helpCMD(const char* s);

shellfunction CMDs[] = {
    CMDENTRY(&helpCMD, "help", "Shows command list", "help"),
    CMDENTRY(&echo, "echo", "Prints text", "echo [text]"),
    CMDENTRY(&clear, "clear", "Clears the screen", "clear"),
    CMDENTRY(&hcf, "hcf", "Crashes your system", "hcf"),
    CMDENTRY(&diskinfo, "diskinfo", "Shows disk info", "diskinfo"),
    CMDENTRY(&diskread, "diskread", "Reads from disk", "diskread [LBA] [num_sectors]"),
    CMDENTRY(&dir, "dir", "Lists files", "dir"),
    CMDENTRY(&newfile, "newfile", "Creates new file", "newfile [name]"),
    CMDENTRY(&rmfile, "rmfile", "Removes file", "rmfile [name]"),
    CMDENTRY(&write_file, "writefile", "Writes string to file", "writefile [filename] [string]"),
    CMDENTRY(&read_file, "readfile", "Reads string from file", "readfile [filename]"),
    CMDENTRY(&texted, "texted", "Opens text editor", "texted [filename]"),
    CMDENTRY(&exec, "exec", "Runs an executable", "exec [filename]"),
    CMDENTRY(&erase_files, "erasefiles", "Erases the file bitmap", "erasefiles"),
#ifdef CONFIG_GFX_VESA
    CMDENTRY(&vell, "vell", "VESA graphic shell", "vell"),
    CMDENTRY(&frogues, "frogues", "Frog viewing program", "frogues"),
    CMDENTRY(&jb_juggle_balls, "jb", "Ball juggling program", "juggle_balls"),
    CMDENTRY(&sperkaster, "sp", "Perk raycasting engine", "sperkaster"),
#endif
    CMDENTRY(&displayinfo, "displayinfo", "Display display info", "displayinfo"),
    CMDENTRY(&meminfo, "meminfo", "Displays memory info", "meminfo"),
    CMDENTRY(&mountcmd, "mount", "Mounts a filesystem", "mount [filesystem] [mountpoint]"),
	CMDENTRY(&ls, "ls", "Lists files", "ls"),
	CMDENTRY(&lsblk, "lsblk", "Lists block devices", "lsblk"),
	CMDENTRY(&pwd, "pwd", "Prints current working directory", "pwd"),
	CMDENTRY(&cd, "cd", "Changes current working directory", "cd [directory]"),
};

void helpCMD(const char* s) {
	if (strlen(s) == 0) {
		kprint("List of commands:\n");
		for (int i = 0; i < sizeof(CMDs) / sizeof(shellfunction); i++) {
			kprint("   ");
			kprint(CMDs[i].usage);
			kprint(": ");
			kprint(CMDs[i].help);
			kprint("\n");
		}
	} else {
		kprint("Invalid option: \"");
		if (s[0] == ' ')
			kprint((const char*)(s + 1));
		else
			kprint(s);
		kprint("\"");
	}
}

shellfunction shellf(void (*Fptr)(const char*), char* Alias, char* Help) {
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

shellfunction* TryGetCMD(char* cmdbuf) {
	for (int x = 0; x < sizeof(CMDs) / sizeof(shellfunction); x++) {
		if (strcmp(cmdbuf, CMDs[x].alias) == 0) {
			return &CMDs[x];
		}
	}
	return 0;
}
