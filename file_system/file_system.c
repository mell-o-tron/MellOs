#include "dynamic_mem.h"
#include "disk.h"
#include "file_system.h"
#include "stddef.h"
#include "string.h"
#include "diskinterface.h"
#include "bitmap.h"
#include "allocator.h"

void prepare_disk_for_fs (uint32_t n_sectors){
    if(n_sectors < 3){
        kprint("There should be at least three sectors");
        for(;;);
    }

    uint16_t *tmp = kmalloc(sizeof(uint16_t) * 256 * n_sectors);
    for(uint32_t i = 0; i < 256 * n_sectors; i++){
        tmp[i] = 0;
    }
    LBA28_write_sector(0xA0, 1, n_sectors, tmp);
    kfree(tmp);
    
    bitmap_t bitmap = kmalloc(512);
    for(uint32_t i = 0; i < 512; i++){
        bitmap[i] = 0;
    }
    
    set_bitmap(bitmap, 0);
    set_bitmap(bitmap, 1);
    set_bitmap(bitmap, 2);
    write_string_to_disk((char*)bitmap, 0xA0, 2, 1);
}

int allocate_file(uint32_t req_sectors){

    char *tmp = (char*)read_string_from_disk(0xA0, 2, 1);
    
    allocator_t allocator;
    allocator.bitmap = (bitmap_t)tmp;
    allocator.size = 512;
    allocator.granularity = 1;
    
    void* res = allocate(&allocator, req_sectors);
    if (res != NULL)
        write_string_to_disk(tmp, 0xA0, 2, 1);
    else
        return -1;
    
    return (int)res;
}

int deallocate_file(uint32_t LBA, uint32_t num_sectors){
    char *tmp = (char*)read_string_from_disk(0xA0, 2, 1);
    allocator_t allocator;
    allocator.bitmap = (bitmap_t)tmp;
    allocator.size = 512;
    allocator.granularity = 1;
    
    int res = allocator_free(&allocator, LBA, num_sectors);
    if (res >= 0)
        write_string_to_disk(tmp, 0xA0, 2, 1);
    else
        return -1;
    
    return res;
}

file_t* get_file_list (uint32_t disk, uint32_t LBA, uint32_t sectors){
    file_t* res = kmalloc(sizeof(file_t) * 32 * sectors);

    // initialize res to all zeroes, to allow non-full result
    for (uint32_t i = 0; i < 32 * sectors; i++){
        for (uint32_t j = 0; j < 10; j++)
            res[i].name[j] = 0;
        res[i].LBA = 0;
        res[i].n_sectors = 0;
    }

    // read disk
    uint16_t *tmp = kmalloc(sizeof(uint16_t) * 256 * sectors);
    LBA28_read_sector(disk, LBA, sectors, tmp);

    // global index
    uint32_t index = 0;

    for (uint32_t i = 0; i < 32 * sectors; i++){
        char* name = kmalloc (10);
        uint32_t LBA_read = 0;
        uint16_t n_sectors_read = 0;

        // retrieve name
        for (uint32_t j = 0; j < 5; j++){
            name[2*j]       = (char)tmp[index + j];
            name[2*j+1]     = (char)(tmp[index + j] >> 8);
        }

        // if name begins with empty char, there are no following names.
        if (name[0] == 0) break;

        // reconstructing LBA (lsb first)
        LBA_read = (uint32_t)tmp[index + 5]
                 | ((uint32_t)tmp[index + 6]) << 16;

        n_sectors_read = tmp[index + 7];

        index += 8;

        // can't be assigned directly ugh
        for (uint32_t j = 0; j < 10; j++){
            res[i].name[j] = name[j];
        }

        res[i].LBA = LBA_read;
        res[i].n_sectors = n_sectors_read;
    }

    kfree(tmp);

    return res;
}

void write_file_list (file_t * list, uint32_t disk, uint32_t LBA, uint32_t sectors){
    uint16_t *tmp = kmalloc(sizeof(uint16_t) * 256 * sectors);

     // global index
    uint32_t index = 0;

    for (uint32_t i = 0; i < 32 * sectors; i++){
        for (uint32_t j = 0; j < 5; j++){
            tmp[j + index] = list[i].name[2*j] | ((uint16_t)list[i].name[2*j + 1] << 8);
        }

        tmp[index + 5] = (uint16_t)list[i].LBA;
        tmp[index + 6] = (uint16_t)(list[i].LBA >> 16);
        tmp[index + 7] = list[i].n_sectors;

        index += 8;
    }

    LBA28_write_sector(disk, LBA, sectors, tmp);
    kfree(tmp);
}

void new_file (char* name, uint32_t n_sectors){
    if(name[0] == 0 || name[0] == ' ' || name[0] == '\n'){
        kprint("Filename is not valid, file not created.\n");
        return;
    }

    file_t * files = get_file_list(0xA0, 1, 1); //TODO should free this

    // check is name already taken & find position for file

    int new_file_pos = -1;
    for (uint32_t i = 0; i < 32; i++){
        if(strcmp(name, files[i].name) == 0) {
            kprint("File with this name already exists.\n");
            goto cleanup;
        }
        if(files[i].name[0] == 0){
            new_file_pos = i;
            break;
        }
    }

    if(new_file_pos == -1){
        kprint("Not enough space for new file\n");
        goto cleanup;
    }

    int32_t LBA = allocate_file(n_sectors);
    
    if(LBA < 0) {
        kprint("Could not allocate file\n");
        goto cleanup;
    }

    // WARNING name is truncated here, so we can still end up with files with same name
    // can't be bothered to fix now, too late at night.

    for (uint32_t j = 0; j < 10; j++){
        files[new_file_pos].name[j] = name[j];
        if(name[j] == 0) break;  // name could be less than 10 long. If >10, it is cut.
    }

    files[new_file_pos].LBA = LBA;
    files[new_file_pos].n_sectors = n_sectors;
    
    write_file_list(files, 0xA0, 1, 1);

cleanup:
    kfree(files);
}

void remove_file(char* name){
    if(name[0] == 0 || name[0] == ' ' || name[0] == '\n'){
        kprint("filename is not valid.\n");
        return;
    }

    file_t * files = get_file_list(0xA0, 1, 1); //TODO should free this

    for (uint32_t i = 0; i < 32; i++){
        if(strcmp(name, files[i].name) == 0){
            int res = deallocate_file(files[i].LBA, files[i].n_sectors);
            
            if(res < 0) {
                kprint("Could not delete file");
                return;
            }
            
            for (uint32_t j = i+1; j < 32; j++){
                files[j - 1] = files[j];
            }
            write_file_list(files, 0xA0, 1, 1);
            return;
        }
    }
}


void print_files(file_t * files, uint32_t n){
    for (uint32_t i = 0; i < n; i++){
        if (files[i].name[0] == 0) break;

        files[i].name[9] = 0;
        kprint(files[i].name);
        kprint(" - LBA: ");
        kprint(tostring_inplace(files[i].LBA, 10));
        kprint(", n. sectors: ");
        kprint(tostring_inplace(files[i].n_sectors, 10));
        kprint("\n");
    }
}

// WARNING these allow to write arbitrarily large strings, there should be a check

void write_string_to_file(char* string, char* filename){
    file_t * files = get_file_list(0xA0, 1, 1); //TODO should free this

    for (uint32_t i = 0; i < 32; i++){
        if(strcmp(filename, files[i].name) == 0){
            write_string_to_disk(string, 0xA0, files[i].LBA, files[i].n_sectors);
            return;
        }
    }
    kprint("file: \"");
    kprint(filename);
    kprint("\" not found\n");

}

char* read_string_from_file(char* filename){
    file_t * files = get_file_list(0xA0, 1, 1); //TODO should free this
    char* res;
    bool found = false;
    for (uint32_t i = 0; i < 32; i++){
        if(strcmp(filename, files[i].name) == 0){
            res = (char*)read_string_from_disk(0xA0, files[i].LBA, files[i].n_sectors);
            found = true;
            break;
        }
    }

    if(!found){
        kprint("file: \"");
        kprint(filename);
        kprint("\" not found\n");
        return NULL;
    }

    return res;
}
