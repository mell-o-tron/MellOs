#include "../../Drivers/VGA_Text.h"
#include "../Memory/mem.h"
#include "../Memory/cmos_memsize.h"
#include "../Utils/Conversions.h"
#define FDEF(name) void name(const char* s)

FDEF(usedmem){
    kprint("Used dynamic memory: ");
    kprint(toString(getFreeMem() - 0x10000, 10));
    kprint(" bytes");
}

FDEF(meminfo){
    kprint("meminfo:\n");
    mem_t mem = memget();
    kprint("Lower memory: ");
    kprint(toString(mem.lower_size, 10));
    kprint(" KB\n");
    kprint("Upper memory: ");
    kprint(toString(mem.upper_size * 64, 10)); //mem.upper_size contains the size in 64K blocks
    kprint(" KB\n");
}

FDEF(cmosmem){
    kprint("cmosmem:\n\t");
    unsigned char mem_size = get_CMOS_memory_size();
    kprint(toString(mem_size, 10)); //mem.upper_size contains the size in 64K blocks
    kprint(" KB\n");
}
