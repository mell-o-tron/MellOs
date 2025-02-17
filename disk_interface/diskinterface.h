#pragma once

#include "../utils/typedefs.h"

uint8_t* read_string_from_disk (uint8_t disk, uint32_t LBA, uint32_t num_sectors);
void write_string_to_disk(char* str, uint8_t disk, uint32_t LBA, uint32_t num_sectors);
