#pragma once
#include "../Utils/Typedefs.h"
#include "./port_io.h"
#include "../CPU/Timer/timer.h"
#include "VGA_Text.h"
#include "../Utils/Conversions.h"

void lba28_read_sector(uint32_t block, uint32_t LBA, uint8_t drive);
void lba28_write_sector(uint8_t drive, uint32_t LBA, uint32_t* buffer);