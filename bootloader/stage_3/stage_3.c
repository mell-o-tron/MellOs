#include "stage_3_miniheader.h"
#include "floppy.h"

// IMPORTANT: These values will need to change as the kernel grows. Eventually this should be automated via makefile. Eventually eventually, alas the floppy will not be adequate at all and someone will need to implement an ATA/PATA/SATA/USB/NVMe/Whatever driver.
#define KERNEL_LOCATION 0x400000
#define KERNEL_SECTORS 256
#define KERNEL_START_SECTOR 16

void main(){
	uint8_t disk = init_floppy();
    char buf[256];
    for(size_t i = 0; i < KERNEL_SECTORS; i++){
        read_floppy_lba(disk, KERNEL_START_SECTOR + i, (uint8_t*)(KERNEL_LOCATION + i * 512));
        // For some reason reading the floppy without resetting it after each read misaligns sectors. Keep this until a better solution is found. Doesn't cause particular delays or issues on qemu anyway.
        drive_setup();
    }

    set_motor(0, 0); // Turn off the motor after reading everything there is to read.
}