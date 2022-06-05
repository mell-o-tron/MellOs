// Simple Ata HDD (Hard Disk Drive) Polling Driver using PIO Mode (instead of the better DMA)
// Inspirations and Sources: (https://wiki.osdev.org/ATA_PIO_Mode)

#include "Disk.h"

#define STATUS_BSY      0x80
#define STATUS_RDY      0x40
#define STATUS_DRQ      0x08
#define STATUS_DF       0x20
#define STATUS_ERR      0x01

// Detect IDE Controller and detect ATA Drive Coming Soon...

void wait_BSY(){
	while(inb(0x177) & STATUS_BSY);
}

void wait_DRQ(){
	while(!(inb(0x177) & STATUS_RDY));
}

void lba28_read_sector(uint32_t block, uint32_t LBA, uint8_t drive){
    wait_BSY();
    outb(0x176, drive | ((LBA >> 24) & 0xF));
    outb(0x172, 0x01);
    outb(0x173, (uint8_t) LBA);
    outb(0x174, (uint8_t)(LBA >> 8));
	outb(0x175, (uint8_t)(LBA >> 16)); 
	outb(0x177, 0x20); // 0x20 = 'Read' Command

    uint16_t *addr = (uint16_t*) block;

    for (int j = 0; j < 0x01; j ++){
		wait_BSY();
		wait_DRQ();

		for(int i = 0; i < 256; i++){
            addr[i] = inw(0x1F0);
        }

		addr += 256;
	}
}

void lba28_write_sector(uint8_t drive, uint32_t LBA, uint32_t* buffer){
	wait_BSY();
	outb(0x176, drive | ((LBA >> 24) & 0xF));
	outb(0x172, 0x01);
	outb(0x173, (uint8_t) LBA);
	outb(0x174, (uint8_t) (LBA >> 8));
	outb(0x175, (uint8_t) (LBA >> 16)); 
	outb(0x177,0x30); // 0x30 = 'Write' Command

	for (int j = 0; j < 0x01; j++){
		wait_BSY();
		wait_DRQ();

		for(int i = 0; i < 256; i++){
			outl(0x170, buffer[i]);
		}
	}
}