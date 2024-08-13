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

// BSY = 1   =>    drive is preparing to send/receive data
void wait_BSY(){
	while(inb(0x1F7) & STATUS_BSY);
}

// DRQ = 1   =>    drive has data to transfer / is ready to accept data
void wait_DRQ(){
	while(!(inb(0x1F7) & STATUS_RDY));
}


uint16_t* LBA28_read_sector(uint8_t drive, uint32_t LBA, uint8_t sector_count, uint16_t *addr){
	
	// turn LBA into 28-bit value
	LBA = LBA & 0x0FFFFFFF;
	
	// wait for driver to get ready to receive data
    wait_BSY();
	
	
	// first send drive number to 0x1F6. Drive number is 0xE0 for MASTER and 0xF0 for SLAVE.
	// the value should be ORd with highest 4 bits of LBA
    outb(0x1F6, drive | ((LBA >> 24) & 0xF));
	
	// Then send null byte to port 0x1F1 (supposedly ignored idk)
	outb(0x1F1, 0x00);
	
	// then send sector count to 0x1F2
    outb(0x1F2, sector_count);
	
	// then send 24 bits of LBA address starting from lowest 8 bits
    outb(0x1F3, (uint8_t) LBA);
    outb(0x1F4, (uint8_t)(LBA >> 8));
	outb(0x1F5, (uint8_t)(LBA >> 16)); 
	
	// then send READ SECTORS command
	outb(0x1F7, 0x20); // 0x20 = 'Read' Command

	
	uint16_t *tmp = addr;
	
    for (int j = 0; j < sector_count; j ++){
		
		// wait for driver to get ready to receive and accept data
		
		wait_BSY();
		wait_DRQ();
		
		// Transfer 256 16-bit values, a uint16_t at a time, into your buffer from I/O port 0x1F0.
		
		for(int i = 0; i < 256; i++){
            tmp[i] = inw(0x1F0);
        }
		
		
		tmp += 256;
		
// 		// should sleep for 400ns, we sleep for .056 seconds. FIXME WTF, sleep does not work when function called from shell!?!?
		// sleep(1);
	}
	return addr;
}
/*

 void LBA28_write_sector(uint8_t drive, uint32_t LBA, uint32_t sector, uint16_t *buffer){
	kprint("\nwriting: ");
	kprint(toString(sector, 10));
	kprint(" sectors at LBA: ");
	kprint(toString(LBA, 10));
	kprint("\n");
	
	LBA28_write_sector_logic(drive, LBA + 1, sector + 1, buffer);
 }*/


// NOTE this for some reason wants LBA + 1 and sector + 1
void LBA28_write_sector(uint8_t drive, uint32_t LBA, uint32_t sector_count, uint16_t *buffer){
	LBA = LBA & 0x0FFFFFFF;
	
	wait_BSY();
	
	outb(0x1F6, drive | ((LBA >> 24) & 0xF));		// send drive and bits 24 - 27 of LBA
	outb(0x1F1, 0x00);								// supposedly ignored
	outb(0x1F2, sector_count);						// send number of sectors
	outb(0x1F3, (uint8_t) LBA);						// send bits 0-7 of LBA
	outb(0x1F4, (uint8_t) (LBA >> 8));				// 8-15
	outb(0x1F5, (uint8_t) (LBA >> 16)); 			// 16-23
	outb(0x1F7,0x30); 								// 0x30 = 'Write' Command

	
	for (int j = 0; j < sector_count; j++){
		int offset = 256 * j;
		wait_BSY();
		wait_DRQ();

		for(int i = 0; i < 256; i++){
			outw(0x1F0, buffer[i + offset]);
		}

		outb(0x1F7, 0xE7);
		// FIXME WTF, sleep does not work when function called from shell!?!?
		//sleep(1);
		
	}
	outb(0x1F7,0xe7);
	wait_BSY();
}


raw_disk_info retrieve_disk_info(){
	raw_disk_info* dinfo = (raw_disk_info*)0x5200;
	return *dinfo;
}

void decode_raw_disk_info(raw_disk_info dinfo, disk_info * result){
	result -> drivetype = dinfo.bl;
	result -> sectors = dinfo.cl & 0b00111111;
	result -> cylinders = ((dinfo.cl & 0b11000000) << 2) | dinfo.ch;
    result -> heads = dinfo.dh;
    result -> drives = dinfo.dl;
}
