#pragma once

typedef struct {
    char*       file_name;
    char        type;
    char        permissions;
    int         first_sector;
    int         num_sectors;
} file_mmd;

int recognise_PFS(int disk);

file_mmd ** get_root_files (int disk, int** file_num);

void list_files (file_mmd ** mmd, int number);

void write_mmd_list (int disk, file_mmd ** mmd, int number);

maybe_void new_file (int disk, char* name, char type, char permissions, int first_sector, int num_sectors, bitmap_t disk_bitmap, int bitmap_size);

maybe_void initial_file (int disk, char* name, char type, char permissions, int first_sector, int num_sectors, bitmap_t disk_bitmap, int bitmap_size);

maybe_int get_free_sector(int size, bitmap_t disk_bitmap, int bitmap_size);

maybe_void new_file_alloc(int disk, char* name, char type, char permissions, int num_sectors, bitmap_t disk_bitmap, int bitmap_size);

maybe_void initial_file_alloc(int disk, char* name, char type, char permissions, int num_sectors, bitmap_t disk_bitmap, int bitmap_size);

maybe_void write_file (int disk, file_mmd* file, uint16_t* contents, int content_size);
maybe_void read_file (int disk, file_mmd* file, uint16_t* contents, int content_size);
