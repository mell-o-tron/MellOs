#pragma once

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

FHDR(ls);
FHDR(newfile);
FHDR(write_file);
FHDR(read_file);
FHDR(rmfile);
FHDR(erase_files);

// text_editor.c

FHDR(texted);

//exec.c

FHDR(exec);

#ifdef VGA_VESA
// vell.c
FHDR(vell);
FHDR(frogues);
#endif
FHDR(displayinfo);
