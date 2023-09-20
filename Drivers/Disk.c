// Simple (P)Ata HDD (Hard Disk Drive) Polling Driver using PIO Mode (instead of the better DMA)
// Inspirations and Sources: (https://wiki.osdev.org/ATA_PIO_Mode)

#include "Disk.h"

#define STATUS_BSY      0x80
#define STATUS_RDY      0x40
#define STATUS_DRQ      0x08
#define STATUS_DF       0x20
#define STATUS_ERR      0x01

void identify_ata(uint8_t drive){
	// 0xA0 for Master
	// 0xB0 for Slave

	outb(0x1F6, drive);
	outb(0x1F2, 0);
	outb(0x1F3, 0);
	outb(0x1F4, 0);
	outb(0x1F5, 0);
	outb(0x1F7, 0xEC);
	uint8_t tmp = inb(0x1F7);
	sleep(2);
	if(tmp & STATUS_RDY){
		switch(drive){
			case 0xA0:
				kprintCol("Master Drive is the current active Drive \n", DARK_COLOR);
				break;
			case 0xB0:
				kprintCol("Slave Drive is the current active Drive \n", DARK_COLOR);
				break;
		}
	}
	else
	{
		switch(drive){
			case 0xA0:
				kprintCol("Master Drive is NOT the current active Drive \n", ERROR_COLOR);
				break;
			case 0xB0:
				kprintCol("Slave Drive is NOT the current active Drive \n", ERROR_COLOR);
				break;
			}
	}
}

// TODO implement something on the lines of "took too long to respond"
void wait_BSY(){
	while(inb(0x1F7) & STATUS_BSY);
}

void wait_DRQ(){
	while(!(inb(0x1F7) & STATUS_RDY));
}

uint16_t* LBA28_read_sector(uint8_t drive, uint32_t LBA, uint32_t sector, uint16_t *addr){
	LBA = LBA & 0x0FFFFFFF;
	
    wait_BSY();
    outb(0x1F6, drive | ((LBA >> 24) & 0xF));
	outb(0x1F1, 0x00);
    outb(0x1F2, sector);
    outb(0x1F3, (uint8_t) LBA);
    outb(0x1F4, (uint8_t)(LBA >> 8));
	outb(0x1F5, (uint8_t)(LBA >> 16)); 
	outb(0x1F7, 0x20); // 0x20 = 'Read' Command

	
	uint16_t *tmp = addr;
	
    for (int j = 0; j < sector; j ++){
		wait_BSY();
		wait_DRQ();
		for(int i = 0; i < 256; i++){
            tmp[i] = inw(0x1F0);
        }

		tmp += 256;
	}
	return addr;
}


// WARNING this only writes the lowest 8 bits of what's in the buffer, and leaves gaps. Idk if this can be done another way, in such case please look into it.
void LBA28_write_sector(uint8_t drive, uint32_t LBA, uint32_t sector, uint16_t *buffer){
	
	kprint("\nwriting: ");
	kprint(toString(sector, 10));
	kprint(" sectors at LBA: ");
	kprint(toString(LBA, 10));
	kprint("\n");
	
	
	LBA = LBA & 0x0FFFFFFF;
	
	wait_BSY();
	outb(0x1F6, drive | ((LBA >> 24) & 0xF));		// send drive and bits 24 - 27 of LBA
	outb(0x1F1, 0x00);								// ?
	outb(0x1F2, sector);							// send number of sectors
	outb(0x1F3, (uint8_t) LBA);						// send bits 0-7 of LBA
	outb(0x1F4, (uint8_t) (LBA >> 8));				// 8-15
	outb(0x1F5, (uint8_t) (LBA >> 16)); 			// 16-23
	outb(0x1F7,0x30); 								// 0x30 = 'Write' Command

	uint32_t *tmp = buffer;
	
	for (int j = 0; j < sector; j++){
		wait_BSY();
		wait_DRQ();

		for(int i = 0; i < 256; i++){
			outl(0x1F0, tmp[i]);
		}

		outb(0x1F7, 0xE7);
		sleep(1);
		
		tmp += 256;
	}
}


disk_info retrieve_disk_info(){
	disk_info* dinfo = (disk_info*)0x5200;
	return *dinfo;
}
