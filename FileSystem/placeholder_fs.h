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
