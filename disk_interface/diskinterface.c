#include "../drivers/disk.h"
#include "../memory/dynamic_mem.h"

uint8_t* read_string_from_disk (uint8_t disk, uint32_t LBA, uint32_t num_sectors){

    uint16_t * buf = (uint16_t*)kmalloc(sizeof(uint16_t) * 256 * num_sectors);
    uint8_t * res  = (uint8_t*)kmalloc(512 * num_sectors + 1);
    res[512 * num_sectors] = 0;

    LBA28_read_sector(0xA0, LBA, num_sectors, buf);
    for (int i = 0; i < 256 * num_sectors; i++){
        res[2 * i]      = (uint8_t)(buf[i] >> 8);
        res[2 * i+1]    = (uint8_t)(buf[i]);
    }

    kfree(buf, sizeof(uint16_t) * 256 * num_sectors);

    return res;
}

void write_string_to_disk(char* str, uint8_t disk, uint32_t LBA, uint32_t num_sectors){

    uint16_t * buf = (uint16_t*)kmalloc(sizeof(uint16_t) * 256 * num_sectors);

    for (int i = 0; i < 256 * num_sectors; i++){
        buf[i] = ((uint16_t)(uint8_t)str[2*i] << 8) | (uint8_t)str[2*i+1];
    }
    LBA28_write_sector(0xA0, LBA, num_sectors, buf);

    kfree(buf, sizeof(uint16_t) * 256 * num_sectors);
}
