#pragma once
#include "../Utils/Typedefs.h"
#include "./port_io.h"
#include "../CPU/Timer/timer.h"
#include "VGA_Text.h"
#include "../Utils/Conversions.h"
#include "../Misc/colors.h"

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

void identify_ata(uint8_t drive);
uint16_t* LBA28_read_sector(uint8_t drive, uint32_t LBA, uint8_t sector_count, uint16_t *addr);
void LBA28_write_sector(uint8_t drive, uint32_t LBA, uint32_t sector_count, uint16_t *buffer);

void decode_raw_disk_info(raw_disk_info dinfo, disk_info * result);
raw_disk_info retrieve_disk_info();
