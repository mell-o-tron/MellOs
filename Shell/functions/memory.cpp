#include "../../Drivers/VGA_Text.h"
#include "../Memory/mem.h"
#include "../Utils/Conversions.h"
#define FDEF(name) void name(const char* s)

FDEF(usedmem){
    kprint("Used dynamic memory: ");
    kprint(toString(getFreeMem() - 0x10000, 10));
    kprint(" bytes");
}