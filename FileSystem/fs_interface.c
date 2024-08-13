#include "../Drivers/Disk.h"
#include "../Memory/dynamic_mem.h"
#include "placeholder_fs.h"
#include "../Utils/Typedefs.h"
#include "../Utils/error_handling.h"
#include "../Utils/Conversions.h"
#include "../Utils/bitmap.h"
#include "../Utils/string.h"
#include "../Misc/colors.h"
#include "../Drivers/VGA_Text.h"
#include "./placeholder_fs.h"

// sector of current directory
extern int CURRENT_DIRECTORY;


maybe_void change_directory (file_mmd* file){
    if (file->type == 1) {
        CURRENT_DIRECTORY = file -> first_sector;
        succeed();
    }
    else {
        fail(0);    // not a directory
    }
}


void list_files_debug (file_mmd ** mmd, int number){
    for (int i = 0; i < number; i++){
        kprint("file name: ");
        kprint(mmd[i] -> file_name);
        kprint("\nfile type: ");
        kprint(toString(mmd[i] -> type, 16));
        kprint("\nfile permissions: ");
        kprint(toString(mmd[i] -> permissions, 16));
        kprint("\nfirst sector: ");
        kprint(toString(mmd[i] -> first_sector, 16));
        kprint("\nnumber of sectors: ");
        kprint(toString(mmd[i] -> num_sectors, 16));
        kprint("\n\n");
    }
}

maybe_void make_directory (uint8_t disk, char* name, bitmap_t disk_bitmap, int bitmap_size) {
    

    int pos = msg_on_fail(new_file_alloc (disk, name, 1, 0, SECTORS_PER_DIRECTORY, disk_bitmap, bitmap_size), "allocation of new directory failed");
    
    
    int prev_pos = CURRENT_DIRECTORY;
    CURRENT_DIRECTORY = pos;
    
    initial_file (disk, "..", 1, 0, prev_pos, 0, disk_bitmap, bitmap_size);
    CURRENT_DIRECTORY = prev_pos;
    succeed();
}
