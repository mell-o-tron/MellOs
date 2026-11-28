#include "disk.h"
#include "port_io.h"
#include "stddef.h"
#include "pata_internal.h"
#include <stdint.h>

int LBA48_read_sector(disk_info_t* disk, uint64_t LBA, uint16_t sector_count, uint16_t* addr) {
	uint8_t drive = disk->id;
	identify_ata(drive, NULL);

	// filter allows up to 2^48
	LBA = LBA & 0xFFFFFFFFFFFF;

	wait_BSY(drive);
	if (drive == DRIVE_SELECT_MASTER) {
		outb(0x1F6, 0x40);
	} else if (drive == DRIVE_SELECT_SLAVE) {
		outb(0x1F6, 0x50);
	}

	ata_delay_400ns();
	// delay
	outb(0x1F1, 0x00);
	outb(0x1F2, sector_count >> 8);
	outb(0x1F3, (uint8_t)(LBA >> 24));
	outb(0x1F4, (uint8_t)(LBA >> 32));
	outb(0x1F5, (uint8_t)(LBA >> 48));
	outb(0x1F2, sector_count);
	outb(0x1F3, (uint8_t)(LBA >> 0));
	outb(0x1F4, (uint8_t)(LBA >> 8));
	outb(0x1F5, (uint8_t)(LBA >> 16));
	outb(0x1F7, 0x24); // 0x24 = 'Read sectors ext' Command
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

// TODO
int LBA48_write_sector(disk_info_t* disk, uint64_t LBA, uint16_t sector_count, uint16_t* buffer) {
	uint8_t drive = disk->id;
	identify_ata(drive, NULL);

	// filter allows up to 2^48
	LBA = LBA & 0xFFFFFFFFFFFF;

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