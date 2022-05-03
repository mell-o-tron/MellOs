#ifndef SHELLFUNCTIONS2_H
#define SHELLFUNCTIONS2_H

// Function header to simplify declaring shell commands
#define FHDR(name) void name(const char* s);

// text.cpp
FHDR(echo);
FHDR(clear);

// memory.cpp
FHDR(usedmem);

// floppy.cpp
FHDR(floppy);

// misc.cpp
FHDR(hcf)

#endif