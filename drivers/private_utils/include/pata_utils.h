#pragma once
#include "disk.h"


void pata_decode_raw_disk_info(raw_disk_info_t raw_disk_info, disk_info_t* result);
raw_disk_info_t pata_retrieve_raw_disk_info();

void LBA28_read_sector(uint8_t drive, uint32_t LBA, uint32_t sector, uint16_t* addr);
void LBA28_write_sector(uint8_t drive, uint32_t LBA, uint32_t sector, uint16_t* buffer);
