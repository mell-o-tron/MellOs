#include "../../Utils/error_handling.h"
#include "../../Utils/Typedefs.h"
#include "../../Utils/bitmap.h"
#include "../../Utils/string.h"
#include "../../FileSystem/placeholder_fs.h"
#include "../../FileSystem/fs_interface.h"
#include "../../Drivers/VGA_Text.h"

#define FDEF(name) void name(const char* s)

extern uint8_t master_drive;
extern bitmap_t disk_bitmap;
extern int bitmap_size;
extern int disk_bitmap_size;

FDEF(change_dir){
    char* name = s + 1;
    int *file_num = 0;
    
    file_mmd **files = get_root_files(master_drive, &file_num);
    for (int i = 0; i < *file_num; i++){
        if (StringsEqu(files[i] -> file_name, name)){
            msg_on_fail(change_directory(files[i]), "cd failed");
            return;
        }
    }
    
    kprint ("No directory with name: \"");
    kprint(name);
    kprint("\"");
}

FDEF(list_files){
    int *file_num = 0;
    file_mmd **files = get_root_files(master_drive, &file_num);
    
    for (int i = 0; i < *file_num; i++) {
        kprint(files[i] -> file_name);
        
        if(files[i] -> type == 1)
            kprint(" (dir)");
        
        kprint("\n");
    }
}

FDEF(make_dir){
    char* name = s + 1;
    
    if (strlen(name) == 0) {
        kprint ("Provide a file name!");
        return;
    }
    
    msg_on_fail(make_directory (master_drive, name, disk_bitmap, disk_bitmap_size), "Directory creation failed");

    
    kprint("Created directory \"");
    kprint(name);
    kprint("\"");
}
