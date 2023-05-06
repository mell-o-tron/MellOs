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

void wait_BSY(){
	while(inb(0x1F7) & STATUS_BSY);
}

void wait_DRQ(){
	while(!(inb(0x1F7) & STATUS_RDY));
}

uint16_t* LBA28_read_sector(uint8_t drive, uint32_t LBA, uint32_t sector, uint16_t *addr){
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

void LBA28_write_sector(uint8_t drive, uint32_t LBA, uint32_t sector, uint16_t *buffer){
	
	// TODO figure out whether the following is needed/works
	uint32_t new_buffer [128 * sector];
	
	for (int i = 0; i < 128; i++){				// 32 bit, else it would not work idk
		new_buffer[i] = 0;
		new_buffer[i] |= buffer[2 * i] & 0xFFFF;
		new_buffer[i] |= (buffer[2 * i + 1] & 0xFFFF) << 16;
	
	}
	
	wait_BSY();
	outb(0x1F6, drive | ((LBA >> 24) & 0xF));
	outb(0x1F1, 0x00);
	outb(0x1F2, sector);
	outb(0x1F3, (uint8_t) LBA);
	outb(0x1F4, (uint8_t) (LBA >> 8));
	outb(0x1F5, (uint8_t) (LBA >> 16)); 
	outb(0x1F7,0x30); // 0x30 = 'Write' Command

	uint32_t *tmp = new_buffer;
	
	for (int j = 0; j < sector; j++){
		wait_BSY();
		wait_DRQ();

		for(int i = 0; i < 128; i++){
			outl(0x1F0, tmp[i]);
		}

		outb(0x1F7, 0xE7);
		sleep(1);
		
		tmp += 128;
	}
}
