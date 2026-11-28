#pragma once
#include "disk.h"
#include "dynamic_mem.h"
#include "mellos/kernel/kernel.h"
#include "stdint.h"
#include "kernel_stdio.h"

void set_lba_data(uint16_t* buf, disk_info_t* diskinfo) {
	diskinfo->data = kzalloc(sizeof(lba_data_t));
	lba_data_t* lbadata = (lba_data_t*)diskinfo->data;

	if (diskinfo->drive == PATA_LBA28) {
		lbadata->sector_count = ((uint32_t)buf[60] << 16 | buf[61]);
	} else if (diskinfo->drive == PATA_LBA48) {
		lbadata->sector_count = ((uint64_t)buf[100] << 48 | (uint64_t)buf[101] << 32 | (uint64_t)buf[102] << 16 | (uint64_t)buf[103] << 0);
	}
	lbadata->sector_count = ((uint32_t)buf[118] << 16) | buf[117];
}

void set_sector_length(uint16_t* infobuffer, disk_info_t* diskinfo) {
	if (!diskinfo) {
		return;
	}
	// FIXME: use some trickery to detect which kind of device we are interacting with,
	// SD cards, USB sticks and other removable storage are usually 512 while HDDs
	// and other "proper storage" is 4096
	diskinfo->sector_size = 512;
}

void read_disk_info(uint8_t disk, disk_info_t* diskinfo) {
	uint16_t* buf = kzalloc(256 * sizeof(uint16_t));

	if (!identify_ata(disk, buf)) {
		kpanic_message("reading info for nonexistent disk");
	}

	if (buf[83] & (uint16_t)(1u << 10)) {
		diskinfo->drive = PATA_LBA48;
		diskinfo->data = kzalloc(sizeof(lba_data_t));
		set_sector_length(buf, diskinfo);
		set_lba_data(buf, diskinfo);
	} else if (buf[60] != 0 || buf[61] != 0) {
		diskinfo->drive = PATA_LBA28;
		diskinfo->data = kzalloc(sizeof(lba_data_t));
		set_sector_length(buf, diskinfo);
		set_lba_data(buf, diskinfo);
	} else {
		diskinfo->drive = PATA_CHS;
		diskinfo->data = kzalloc(sizeof(chs_data_t));
		kprintf("trying to chs, plz fix ur ghetto pc\n");
		// not used
	}
	diskinfo->id = disk;
}