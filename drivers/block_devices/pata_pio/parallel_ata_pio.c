#include "autoconf.h"
// Simple (P)Ata HDD (Hard Disk Drive) Polling Driver using PIO Mode (instead of the better DMA)
// Inspirations and Sources: (https://wiki.osdev.org/ATA_PIO_Mode)

#include "colours.h"
#include "disk.h"
#include "mellos/block_device.h"
#include "stddef.h"
#include "port_io.h"
#include "timer.h"
#include "kernel_stdio.h"
#include "pata_internal.h"
#include "stdint.h"
#include "errno.h"
#ifdef CONFIG_GFX_VESA
#include "vesa_text.h"
#else
#include "vga_text.h"
#endif

#define STATUS_BSY 0x80
#define STATUS_RDY 0x40
#define STATUS_DRQ 0x08
#define STATUS_DF 0x20
#define STATUS_ERR 0x01

void ata_delay_400ns(void) {
#ifdef MELLOS_DEBUG
	kprint("ata_delay_400ns\n");
#endif
	inb(0x3F6);
	inb(0x3F6);
	inb(0x3F6);
	inb(0x3F6);
}

#define DRIVER_NAME "PATA"
#include "macros.h"

uint32_t start_bsy_ms = 0;
int wait_BSY(uint8_t drive_num) {
	start_bsy_ms = get_ms_since_boot();
	while (inb(0x1F7) & STATUS_BSY) {
		if (get_ms_since_boot() - start_bsy_ms > 100) {
			kprintf("ATA%X: BSY timeout\n", drive_num);
			return 1;
		}
	}
	return 0;
}

uint32_t start_drq_ms = 0;
int wait_DRQ(uint8_t drive_num) {
	start_drq_ms = get_ms_since_boot();
	while (!(inb(0x1F7) & STATUS_RDY)) {
		if (get_ms_since_boot() - start_drq_ms > 100) {
			kprintf("ATA%i: DRQ timeout\n", drive_num);
			return 1;
		}
	}
	return 0;
}

bool check_ERR() {
	return ((inb(0x1F7) & STATUS_ERR) != 0);
}

// as specified on https://wiki.osdev.org/ATA_PIO_Mode#IDENTIFY_command
uint16_t* identify_ata(uint8_t drive, uint16_t* buffer) {
	outb(0x1F6, drive);
	ata_delay_400ns();
	outb(0x1F2, 0);
	outb(0x1F3, 0);
	outb(0x1F4, 0);
	outb(0x1F5, 0);

	outb(0x1F7, 0xEC); // send identify command
	ata_delay_400ns();

	uint8_t status = inb(0x1F7);

	if (status == 0) {
#ifdef MELLOS_DEBUG
		kprint("Error: drive does not exist\n");
#endif
		return NULL;
	}

	if (status == 0xFF) {
#ifdef MELLOS_DEBUG
		kprint("Error: floating bus, there are no drives connected.\n");
#endif
		return NULL;
	}

	wait_BSY(drive);

	// check, in case drive does not follow spec
	if ((inb(0x1F4) | inb(0x1F5)) != 0) {
		kprintf("Error: drive is not ATA: %X\n", drive);
		return NULL;
	}
	wait_DRQ(drive);

	if (check_ERR()) {
		kprintf("ATA Identify Error: %X\n", drive);
		return NULL;
	}

	if (!buffer) {
		for (int i = 0; i < 256; i++) {
			inw(0x1F0);
		}
		return NULL;
	}

	for (int i = 0; i < 256; i++) {
		buffer[i] = inw(0x1F0);
	}

	return buffer;
}

void ata_print_error(uint8_t error) {
	kprint("Error details: ");
	if (error & 0x01)
		kprint("AMNF "); // Address Mark Not Found
	if (error & 0x02)
		kprint("TK0NF "); // Track 0 Not Found
	if (error & 0x04)
		kprint("ABRT "); // Aborted Command
	if (error & 0x08)
		kprint("MCR "); // Media Change Request
	if (error & 0x10)
		kprint("IDNF "); // ID Not Found
	if (error & 0x20)
		kprint("MC "); // Media Changed
	if (error & 0x40)
		kprint("UNC "); // Uncorrectable Data
	if (error & 0x80)
		kprint("BBK "); // Bad Block
	kprint("\n");
}

int check_ata_error(void) {
	uint8_t status = inb(0x1F7); // Read the status register

	// Check the ERR bit (bit 0)
	if (status & 0x01) {
		uint8_t error = inb(0x1F1); // Read the error register
		kprintf("ATA command error: status = %X", status);
		kprintf(", error = %X\n", error);
		ata_print_error(error);
		return error;
	} else {
		// kprint("ATA command completed successfully, status = 0x");
		// kprint(tostring_inplace(status, 16));
		// kprint("\n");
	}
	return 0;
}
//	ssize_t (*read_blocks)(block_device_t* dev, uint64_t lba, size_t count, void* buffer);
int atapi_flush(block_device_t* dev) {
	int id = ((disk_device_t*)dev->driver_data)->disk_info->id;
	outb(0x1F6, id);
	ata_delay_400ns();
	if (wait_BSY(id) != 0) {
		return 1;
	}
	// flush
	outb(0x1F7, 0xE7);
	if (wait_BSY(id) != 0) {
		return 1;
	}
	return 0;
}

ssize_t atapi_read(block_device_t* dev, uint64_t lba, size_t count, void* buffer) {
	if ((disk_device_t*)dev->driver_data == NULL) {
        ERR("dev->driver_data is NULL (atapi_read)");
		return -EINVAL;
	}
	if (((disk_device_t*)dev->driver_data)->disk_info == NULL) {
        ERR("dev->driver_data->disk_info is NULL (atapi_read)");
        return -EINVAL;
	}
	if (!(dev->flags & BLOCK_DEVICE_FLAG_DISK_DEVICE)) {
		ERR("trying to read non-disk device with atapio driver!");
        return -EINVAL;
	}
	return read_sector(((disk_device_t*)dev->driver_data)->disk_info, lba, count, buffer);
}

ssize_t atapi_write(block_device_t* dev, uint64_t lba, size_t count, const void* buffer) {
	return write_sector(((disk_device_t*)dev->driver_data)->disk_info, lba, count, (void*)buffer);
}

int write_sector(disk_info_t* disk, uint64_t LBA, uint16_t sector, uint16_t* addr) {
	switch (disk->drive) {
		case PATA_LBA28:
			return LBA28_write_sector(disk, LBA, sector, addr);
		case PATA_LBA48:
			return LBA48_write_sector(disk, LBA, sector, addr);
		case PATA_CHS:
			ERR("chs is not supported! (write_sector)");
	}
	return 0;
}


int read_sector(disk_info_t* disk, uint64_t LBA, uint16_t sector, uint16_t* addr) {
	switch (disk->drive) {
        case PATA_LBA28:
            return LBA28_read_sector(disk, LBA, sector, addr);
        case PATA_LBA48:
            return LBA48_read_sector(disk, LBA, sector, addr);
        case PATA_CHS:
            ERR("CHS is not supported! (read_sector)");
	}
	return 0;
}

