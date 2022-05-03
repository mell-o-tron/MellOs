#include "../../Drivers/VGA_Text.h"
#include "../../Drivers/Floppy.h"
#define FDEF(name) void name(const char* s)

FDEF(floppy){
    floppy_detect_drives();
}