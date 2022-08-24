#ifndef SHELLFUNCTIONS2_H
#define SHELLFUNCTIONS2_H

// Function header to simplify declaring shell commands
#define FHDR(name) void name(const char* s);

// text.c
FHDR(echo);
FHDR(clear);

// memory.c
FHDR(usedmem);
FHDR(meminfo);

// floppy.c
FHDR(floppy);

// misc.c
FHDR(hcf)

#endif