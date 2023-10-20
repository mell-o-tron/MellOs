#include "../../Drivers/VGA_Text.h"
#include "../../Drivers/Disk.h"
#include "../../Memory/dynamic_mem.h"
#define FDEF(name) void name(const char* s)

FDEF(diskinfo){
    disk_info* res = kmalloc (sizeof(disk_info));
    if (!res) {
        kprint("info allocation failed");
        return;
    }
    kprint("WARNING: these results are not correct, there is still something wrong with the disk param BIOS routine\n");
    raw_disk_info dinfo = retrieve_disk_info();
    kprint("raw disk info:");
    kprint("\n  bl: ");
    kprint(toString((unsigned char)dinfo.bl, 2));
    kprint("\n  ch: ");
    kprint(toString((unsigned char)dinfo.ch, 2));
    kprint("\n  cl: ");
    kprint(toString((unsigned char)dinfo.cl, 2));
    kprint("\n  dh: ");
    kprint(toString((unsigned char)dinfo.dh, 2));
    kprint("\n  dl: ");
    kprint(toString((unsigned char)dinfo.dl, 2));
    kprint("\n");
    decode_raw_disk_info(dinfo, res);

    kprint("diskinfo:\n");
    kprint("  Drive type: ");
    switch (res->drivetype){
        case 0x01: kprint("360K"); break;
        case 0x02: kprint("1.2M"); break;
        case 0x03: kprint("720K"); break;
        case 0x04: kprint("1.44M"); break;
        case 0x05: kprint("??? (obscure drive or 2.88M)"); break;
        case 0x06: kprint("2.88M"); break;
        case 0x10: kprint("ATAPI Removable Media Device"); break;
        default: kprint ("unrecognized");
    }
    kprint(toString(res->drivetype, 10));
    kprint("\n  Max Sectors: ");
    kprint(toString(res->sectors, 10));
    kprint("\n  Max heads: ");
    kprint(toString(res->heads, 10));
    kprint("\n  Max cylinders: ");
    kprint(toString(res->cylinders, 10));
    kprint("\n  Connected drives: ");
    kprint(toString(res->drives, 10));
    kprint("\n");
} 
