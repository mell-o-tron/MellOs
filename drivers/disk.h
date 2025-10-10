#pragma once
#include "../utils/typedefs.h"
#include "./port_io.h"
#include "../cpu/timer/timer.h"
#ifdef VGA_VESA
#include "drivers/vesa/vesa_text.h"
#else
#include "drivers/vga_text.h"
#endif
#include "../utils/conversions.h"
#include "../misc/colours.h"

typedef struct{
    char drivetype;
    short sectors;
    short cylinders;
    char heads;
    char drives;
}
disk_info;

typedef struct{
    char bl;
    char ch;
    char cl;
    char dh;
    char dl;
}
raw_disk_info;


uint16_t* identify_ata(uint8_t drive);
void LBA28_read_sector(uint8_t drive, uint32_t LBA, uint32_t sector, uint16_t *addr);
void LBA28_write_sector(uint8_t drive, uint32_t LBA, uint32_t sector, uint16_t *buffer);

void decode_raw_disk_info(raw_disk_info dinfo, disk_info * result);
raw_disk_info retrieve_disk_info();
