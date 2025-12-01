#pragma once
#include "autoconf.h"
#include "stdint.h"
#include "port_io.h"
#include "timer.h"
#ifdef CONFIG_GFX_VESA
#include "vesa_text.h"
#else
#include "vga_text.h"
#endif
#include "conversions.h"
#include "colours.h"

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

typedef struct {
    uint8_t drive;
} hd_disk_info_t;


uint16_t* identify_ata(uint8_t drive);
void LBA28_read_sector(uint8_t drive, uint32_t LBA, uint32_t sector, uint16_t *addr);
void LBA28_write_sector(uint8_t drive, uint32_t LBA, uint32_t sector, uint16_t *buffer);

void decode_raw_disk_info(raw_disk_info dinfo, disk_info * result);
raw_disk_info retrieve_disk_info();
