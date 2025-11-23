#pragma once
#include "autoconf.h"

// Function header to simplify declaring shell commands
#define FHDR(name) void name(const char* s);

// text.c
FHDR(echo);
FHDR(clear);


// misc.c
FHDR(hcf);

// disk.c
FHDR(diskinfo);
FHDR(diskread);

// file_sys.c

FHDR(dir);
FHDR(newfile);
FHDR(write_file);
FHDR(read_file);
FHDR(rmfile);
FHDR(erase_files);

// unix_files.c
FHDR(ls);
FHDR(lsblk);
FHDR(pwd);
FHDR(cd);

// text_editor.c

FHDR(texted);

//exec.c

FHDR(exec);

#ifdef CONFIG_GFX_VESA
// vell.c
FHDR(vell);
FHDR(frogues);
FHDR(jb_juggle_balls);
FHDR(sperkaster);
#endif
FHDR(displayinfo);
FHDR(meminfo);
FHDR(mountcmd);