#pragma once
#include "stdint.h"
void ata_delay_400ns(void);
int wait_DRQ(uint8_t drive_num);
int wait_BSY(uint8_t drive_num);
int check_ata_error(void);