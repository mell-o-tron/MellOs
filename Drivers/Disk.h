#pragma once
#include "../Utils/Typedefs.h"
#include "./port_io.h"
#include "../CPU/Timer/timer.h"
#include "VGA_Text.h"
#include "../Utils/Conversions.h"
#include "../Misc/colors.h"

void identify_ata(uint8_t drive);
uint16_t* LBA28_read_sector(uint8_t drive, uint32_t LBA, uint32_t sector, uint16_t *addr);
void LBA28_write_sector(uint8_t drive, uint32_t LBA, uint32_t sector, uint16_t *buffer);
