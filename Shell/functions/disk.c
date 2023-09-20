#include "../../Drivers/VGA_Text.h"
#include "../../Drivers/Disk.h"
#define FDEF(name) void name(const char* s)

FDEF(diskinfo){
    disk_info dinfo = retrieve_disk_info();
    kprint("diskinfo:\n");
    kprint("  Drive type: ");
    switch (dinfo.drivetype){
        case 0x01: kprint("360K"); break;
        case 0x02: kprint("1.2M"); break;
        case 0x03: kprint("720K"); break;
        case 0x04: kprint("1.44M"); break;
        case 0x05: kprint("??? (obscure drive or 2.88M)"); break;
        case 0x06: kprint("2.88M"); break;
        case 0x10: kprint("ATAPI Removable Media Device"); break;
        default: kprint ("unrecognized");
    }
    kprint(toString(dinfo.drivetype, 10));
    kprint("\n  Sectors per track: ");
    kprint(toString(dinfo.sectors_per_track, 10));
    kprint("\n  Sides: ");
    kprint(toString(dinfo.sides, 10));
    kprint("\n  Connected drives: ");
    kprint(toString(dinfo.drives, 10));  
    kprint("\n");
} 
