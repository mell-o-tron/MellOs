#pragma once
#include "autoconf.h"
#include "stdint.h"

#define DRIVE_SELECT_MASTER 0xA0
#define DRIVE_SELECT_SLAVE 0xB0

typedef struct block_device block_device_t;

typedef enum {
	PATA_LBA48, PATA_LBA28, PATA_CHS
} e_disk_type;

typedef struct {
	uint64_t sector_count;
} lba_data_t;

typedef struct {
	uint8_t sectors;
	uint8_t cylinders;
	uint8_t heads;
} chs_data_t;

typedef struct {
	e_disk_type drive;
	uint8_t id;
	uint32_t sector_size;
	// chs_data_t if drive is PATA_CHS, else lba_data_t
	void* data;
} disk_info_t;

uint16_t* identify_ata(uint8_t drive, uint16_t* buffer);
int LBA28_read_sector(disk_info_t* drive, uint32_t LBA, uint8_t sector, uint16_t* addr);
int LBA28_write_sector(disk_info_t* drive, uint32_t LBA, uint8_t sector, uint16_t* buffer);

int LBA48_read_sector(disk_info_t* drive, uint64_t LBA, uint16_t sector, uint16_t* addr);
int LBA48_write_sector(disk_info_t* drive, uint64_t LBA, uint16_t sector, uint16_t* buffer);

ssize_t atapi_read(block_device_t* dev, uint64_t lba, size_t count, void* buffer);
ssize_t atapi_write(block_device_t* dev, uint64_t lba, size_t count, const void* buffer);

int atapi_flush(block_device_t* dev);

int write_sector(disk_info_t* disk, uint64_t LBA, uint16_t sector, uint16_t* addr);
int read_sector(disk_info_t* drive, uint64_t LBA, uint16_t sector, uint16_t* addr);
/**
 * @param drive use the DRIVE_SELECT_x macros
 * @param buf preallocated buffer where the info is stored
 */
void read_disk_info(uint8_t drive, disk_info_t* buf);
