#pragma once
#include "../Utils/Typedefs.h"
#include "port_io.h"


void floppy_detect_drives();
int floppy_init();
int floppy_read(int drive, uint32_t lba, void* address, uint16_t count);
int floppy_write(int drive, uint32_t lba, void* address, uint16_t count);
