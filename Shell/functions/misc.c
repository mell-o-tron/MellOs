#include "../../Drivers/VGA_Text.h"
#define FDEF(name) void name(const char* s)

FDEF(hcf){
    int x = *((uint8_t*) 0xffffffff) = 1;
}
