#ifdef VGA_VESA
#include "../../drivers/vesa/vesa_text.h"
#else
#include "../../drivers/vga_text.h"
#endif
#include "../../drivers/disk.h"
#include "../../memory/dynamic_mem.h"
#include "../../utils/conversions.h"
#include "../../disk_interface/diskinterface.h"


#define FDEF(name) void name(const char* s)

FDEF(diskinfo){
    identify_ata(0xA0);
    // disk_info* res = kmalloc (sizeof(disk_info));
    // if (!res) {
    //     kprint("info allocation failed");
    //     return;
    // }

    // raw_disk_info dinfo = retrieve_disk_info();
    // kprint("raw disk info:");
    // kprint("\n  bl: ");
    // kprint(tostring_inplace((unsigned char)dinfo.bl, 2));
    // kprint("\n  ch: ");
    // kprint(tostring_inplace((unsigned char)dinfo.ch, 2));
    // kprint("\n  cl: ");
    // kprint(tostring_inplace((unsigned char)dinfo.cl, 2));
    // kprint("\n  dh: ");
    // kprint(tostring_inplace((unsigned char)dinfo.dh, 2));
    // kprint("\n  dl: ");
    // kprint(tostring_inplace((unsigned char)dinfo.dl, 2));
    // kprint("\n");
    // decode_raw_disk_info(dinfo, res);

    // kprint("\ndiskinfo:\n");
    // kprint("  Drive type: ");
    // switch (res->drivetype){
    //     case 0x01: kprint("360K"); break;
    //     case 0x02: kprint("1.2M"); break;
    //     case 0x03: kprint("720K"); break;
    //     case 0x04: kprint("1.44M"); break;
    //     case 0x05: kprint("??? (obscure drive or 2.88M)"); break;
    //     case 0x06: kprint("2.88M"); break;
    //     case 0x10: kprint("ATAPI Removable Media Device"); break;
    //     default: kprint ("unrecognized");
    // }
    // kprint(" - 0x");
    // kprint(tostring_inplace(res->drivetype, 16));
    // kprint("\n  Max Sectors: ");
    // kprint(tostring_inplace(res->sectors, 10));
    // kprint("\n  Max heads: ");
    // kprint(tostring_inplace(res->heads, 10));
    // kprint("\n  Max cylinders: ");
    // kprint(tostring_inplace(res->cylinders, 10));
    // kprint("\n  Connected drives: ");
    // kprint(tostring_inplace(res->drives, 10));
    // kprint("\n");
} 


FDEF(diskread){
    const char* t = s;

    uint32_t LBA = 0;
    uint32_t sectors = 0;

    char LBA_string [11];
    char sectors_string [11];

    for (uint32_t i = 0; i<11; i++){
        LBA_string[i] = 0;
        sectors_string[i] = 0;
    }

    LBA_string[10] = 0;
    sectors_string[10] = 0;

    // skip leading whitespaces
    while ((*t == ' ' || *t == '\t') && (t - s < 128)) {
        t++;
    }

    if(t - s >= 128){
        kprint("Arguments not recognized.\n");
        return;
    }

    uint32_t i = 0;
    while (*t != ' ' && *t != '\t'  && *t != 0 && i < 10) {
        LBA_string[i] = *t;
        t++;
        i++;
    }

    if(*t != ' ' && *t != '\t'){
        kprint("Entered LBA address is too large.\n");
        return;
    }

    // skip spaces again
    while ((*t == ' ' || *t == '\t') && (t - s < 128)) {
        t++;
    }

    if(t - s >= 128){
        kprint("Enter a second argument.\n");
        return;
    }

    i = 0;
    while (*t != ' ' && *t != '\t' && *t != 0 && i < 10) {
        sectors_string[i] = *t;
        t++;
        i++;
    }

    if(*t != ' ' && *t != '\t' && *t != 0){
        kprint("Entered sector number is too large.\n");
        return;
    }

    kprint("Reading disk at LBA ");

    kprint(tostring_inplace(string_to_int_dec(LBA_string),10));
    kprint(", for ");
    kprint(tostring_inplace(string_to_int_dec(sectors_string),10));
    kprint(" sectors:\n");

    uint8_t* res = read_string_from_disk(0xA0, LBA, sectors);
    kprint((const char*) res);
    kfree(res);
    kprint("\n");

}
