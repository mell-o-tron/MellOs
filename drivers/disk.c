// Simple (P)Ata HDD (Hard Disk Drive) Polling Driver using PIO Mode (instead of the better DMA)
// Inspirations and Sources: (https://wiki.osdev.org/ATA_PIO_Mode)

#include "disk.h"
#include "port_io.h"
#ifdef VGA_VESA
#include "drivers/vesa/vesa_text.h"
#else
#include "drivers/vga_text.h"
#endif
#include "dynamic_mem.h"

#define STATUS_BSY      0x80
#define STATUS_RDY      0x40
#define STATUS_DRQ      0x08
#define STATUS_DF       0x20
#define STATUS_ERR      0x01

static inline void ata_delay_400ns(void) {
    inb(0x3F6);
    inb(0x3F6);
    inb(0x3F6);
    inb(0x3F6);
}

// TODO implement something on the lines of "took too long to respond"
void wait_BSY(){
	while(inb(0x1F7) & STATUS_BSY){};
}

void wait_DRQ(){
	while(!(inb(0x1F7) & STATUS_RDY)){};
}

bool check_ERR(){
	return ((inb(0x1F7) & STATUS_ERR) != 0);
}


// as specified on https://wiki.osdev.org/ATA_PIO_Mode#IDENTIFY_command
uint16_t* identify_ata(uint8_t drive){
	outb(0x1F6, drive);
	ata_delay_400ns();
	outb(0x1F2, 0);
	outb(0x1F3, 0);
	outb(0x1F4, 0);
	outb(0x1F5, 0);

	outb(0x1F7, 0xEC); // send identify command
	ata_delay_400ns();
	
	uint8_t status = inb(0x1F7);

	if (status == 0){
		kprint("Error: drive does not exist\n");
		return NULL;
	}

	if (status == 0xFF){
		kprint("Error: floating bus, there are no drives connected.\n");
		return NULL;
	}

	wait_BSY();


	// check, in case drive does not follow spec
	if((inb(0x1F4) | inb(0x1F5)) != 0){
		kprint("Error: drive is not ATA\n");
		return NULL;
	}
	wait_DRQ();

	if (check_ERR()){
		kprint("ATA Identify Error\n");
		return NULL;
	}

	uint16_t * res = kmalloc(sizeof(uint16_t) * 256);

	for (int i = 0; i < 256; i++){
		res[i] = inw(0x1F0);
	}

	return res;

}



void ata_print_error(uint8_t error) {
    kprint("Error details: ");
    if(error & 0x01) kprint("AMNF ");  // Address Mark Not Found
    if(error & 0x02) kprint("TK0NF "); // Track 0 Not Found
    if(error & 0x04) kprint("ABRT ");  // Aborted Command
    if(error & 0x08) kprint("MCR ");   // Media Change Request
    if(error & 0x10) kprint("IDNF ");  // ID Not Found
    if(error & 0x20) kprint("MC ");    // Media Changed
    if(error & 0x40) kprint("UNC ");   // Uncorrectable Data
    if(error & 0x80) kprint("BBK ");   // Bad Block
    kprint("\n");
}


void check_ata_error(void) {
    uint8_t status = inb(0x1F7); // Read the status register

    // Check the ERR bit (bit 0)
    if (status & 0x01) {
        uint8_t error = inb(0x1F1); // Read the error register
        kprint("ATA command error: status = 0x");
        kprint(tostring_inplace(status, 16));
        kprint(", error = 0x");
        kprint(tostring_inplace(error, 16));
        kprint("\n");
        ata_print_error(error);
    } else {
        // kprint("ATA command completed successfully, status = 0x");
        // kprint(tostring_inplace(status, 16));
        // kprint("\n");
    }
}

void LBA28_read_sector(uint8_t drive, uint32_t LBA, uint32_t sector, uint16_t *addr){
	identify_ata(drive);

	LBA = LBA & 0x0FFFFFFF;

    wait_BSY();
    outb(0x1F6, drive | ((LBA >> 24) & 0xF));
	ata_delay_400ns();
	outb(0x1F1, 0x00);
    outb(0x1F2, sector);
    outb(0x1F3, (uint8_t) LBA);
    outb(0x1F4, (uint8_t)(LBA >> 8));
	outb(0x1F5, (uint8_t)(LBA >> 16)); 
	outb(0x1F7, 0x20); // 0x20 = 'Read' Command
	ata_delay_400ns();
	uint16_t *tmp = addr;
	
    for (int j = 0; j < sector; j ++){
		wait_BSY();
		wait_DRQ();
		
		for(int i = 0; i < 256; i++){
            tmp[i] = inw(0x1F0);
        }

		tmp += 256;
	}

	check_ata_error();
}


void LBA28_write_sector(uint8_t drive, uint32_t LBA, uint32_t sector, uint16_t *buffer){
	identify_ata(drive);
	// kprint("\nwriting: ");
	// kprint(tostring_inplace(sector, 10));
	// kprint(" sectors at LBA: ");
	// kprint(tostring_inplace(LBA, 10));
	// kprint("\n");
	
	
	LBA = LBA & 0x0FFFFFFF;
	
	wait_BSY();
	outb(0x1F6, drive | ((LBA >> 24) & 0xF));		// send drive and bits 24 - 27 of LBA
	ata_delay_400ns();
	outb(0x1F1, 0x00);								// ?
	outb(0x1F2, sector);							// send number of sectors
	outb(0x1F3, (uint8_t) LBA);						// send bits 0-7 of LBA
	outb(0x1F4, (uint8_t) (LBA >> 8));				// 8-15
	outb(0x1F5, (uint8_t) (LBA >> 16)); 			// 16-23
	outb(0x1F7,0x30); 								// 0x30 = 'Write' Command
	ata_delay_400ns();

	uint16_t *tmp = buffer;
	
	for (int j = 0; j < sector; j++){
		wait_BSY();
		wait_DRQ();
		
		for(int i = 0; i < 256; i++){
			outw(0x1F0, tmp[i]);
		}

		
		tmp += 256;
	}

	check_ata_error();
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
