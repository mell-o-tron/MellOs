#pragma once

typedef struct {
    char*       file_name;
    char        type;
    char        permissions;
    int         first_sector;
    int         num_sectors;
} file_mmd;

int recognise_PFS(uint8_t disk);

file_mmd ** get_root_files (uint8_t disk, int** file_num);

void write_mmd_list (uint8_t disk, file_mmd ** mmd, int number);

maybe_void new_file (uint8_t disk, char* name, char type, char permissions, int first_sector, int num_sectors, bitmap_t disk_bitmap, int bitmap_size);

maybe_void initial_file (uint8_t disk, char* name, char type, char permissions, int first_sector, int num_sectors, bitmap_t disk_bitmap, int bitmap_size);

maybe_int get_free_sector(int size, bitmap_t disk_bitmap, int bitmap_size);

maybe_int new_file_alloc(uint8_t disk, char* name, char type, char permissions, int num_sectors, bitmap_t disk_bitmap, int bitmap_size);

maybe_void initial_file_alloc(uint8_t disk, char* name, char type, char permissions, int num_sectors, bitmap_t disk_bitmap, int bitmap_size);

maybe_void write_file (uint8_t disk, file_mmd* file, uint16_t* contents, int content_size);
maybe_void read_file (uint8_t disk, file_mmd* file, uint16_t* contents, int content_size);

#define SECTORS_PER_DIRECTORY 1
