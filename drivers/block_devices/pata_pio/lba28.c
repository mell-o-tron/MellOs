#include "disk.h"
#include "port_io.h"
#include "stddef.h"
#include "pata_internal.h"

int LBA28_read_sector(disk_info_t* disk, uint32_t LBA, uint8_t sector_count, uint16_t* addr) {
	uint8_t drive = disk->id;
	identify_ata(drive, NULL);

	LBA = LBA & 0x0FFFFFFF;

	wait_BSY(drive);
	outb(0x1F6, drive | ((LBA >> 24) & 0xF));
	ata_delay_400ns();
	// delay
	outb(0x1F1, 0x00);
	outb(0x1F2, sector_count);
	outb(0x1F3, (uint8_t)LBA);
	outb(0x1F4, (uint8_t)(LBA >> 8));
	outb(0x1F5, (uint8_t)(LBA >> 16));
	outb(0x1F7, 0x20); // 0x20 = 'Read' Command
	ata_delay_400ns();
	uint16_t* tmp = addr;

	for (int j = 0; j < sector_count; j++) {
		wait_BSY(drive);
		wait_DRQ(drive);

		for (int i = 0; i < 256; i++) {
			tmp[i] = inw(0x1F0);
		}

		tmp += 256;
	}

	return check_ata_error();
}

int LBA28_write_sector(disk_info_t* disk, uint32_t LBA, uint8_t sector_count, uint16_t* buffer) {
	uint8_t drive = disk->id;
	identify_ata(drive, NULL);

	LBA = LBA & 0x0FFFFFFF;

	wait_BSY(drive);
	outb(0x1F6, drive | ((LBA >> 24) & 0xF)); // send drive and bits 24 - 27 of LBA
	ata_delay_400ns();
	outb(0x1F1, 0x00);                 // delay
	outb(0x1F2, sector_count);               // send number of sectors
	outb(0x1F3, (uint8_t)LBA);         // send bits 0-7 of LBA
	outb(0x1F4, (uint8_t)(LBA >> 8));  // 8-15
	outb(0x1F5, (uint8_t)(LBA >> 16)); // 16-23
	outb(0x1F7, 0x30);                 // 0x30 = 'Write' Command
	ata_delay_400ns();

	uint16_t* tmp = buffer;

	for (int j = 0; j < sector_count; j++) {
		wait_BSY(drive);
		wait_DRQ(drive);

		for (int i = 0; i < 256; i++) {
			outw(0x1F0, tmp[i]);
		}

		tmp += 256;
	}

	return check_ata_error();
}