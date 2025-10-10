#pragma once
#include "stdint.h"

// 16 bytes, hence 32 file descriptors fit in a sector
typedef struct {
    char name [10];
    uint32_t LBA;
    uint16_t n_sectors;

} file_t;

file_t* get_file_list (uint32_t disk, uint32_t LBA, uint32_t sectors);

void write_file_list (file_t * list, uint32_t disk, uint32_t LBA, uint32_t sectors);
void new_file (char* name, uint32_t n_sectors);
void print_files(file_t * files, uint32_t n);
void prepare_disk_for_fs (uint32_t n_sectors);
void write_string_to_file(char* string, char* filename);
char* read_string_from_file(char* filename);
void remove_file(char* name);
