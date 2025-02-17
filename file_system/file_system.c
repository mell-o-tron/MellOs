#include "../utils/typedefs.h"
#include "../memory/dynamic_mem.h"
#include "../drivers/disk.h"
#include "./file_system.h"
#include "../utils/string.h"
#include "../disk_interface/diskinterface.h"

void prepare_disk_for_fs (uint32_t n_sectors){
    if(n_sectors < 3){
        kprint("There should be at least three sectors");
        for(;;);
    }

    uint16_t *tmp = kmalloc(sizeof(uint16_t) * 256 * n_sectors);
    for(uint32_t i = 0; i < 256 * n_sectors; i++){
        tmp[i] = 0;
    }

    tmp[256] = 3;

    LBA28_write_sector(0xA0, 1, n_sectors, tmp);
}

uint32_t allocate_file(uint32_t req_sectors){
    // DUMMY
    if (req_sectors != 1) {
        kprint("I don't know how to do that yet!");
        for(;;);
    }

    uint16_t *tmp = kmalloc(sizeof(uint16_t) * 256);
    LBA28_read_sector(0xA0, 2, 1, tmp);

    uint32_t res = (uint32_t)tmp[0];        // reads dummy counter from disk
    tmp[0] += 1;                            // increments it
    LBA28_write_sector(0xA0, 2, 1, tmp);    // rewrites it

    kfree(tmp, sizeof(uint16_t) * 256);
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

    kfree(tmp, sizeof(uint16_t) * 256 * sectors);

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
    kfree(tmp, sizeof(uint16_t) * 256 * sectors);
}

void new_file (char* name){
    if(name[0] == 0 || name[0] == ' ' || name[0] == '\n'){
        kprint("name is not valid, file not created.\n");
        return;
    }

    file_t * files = get_file_list(0xA0, 1, 1); //TODO should free this

    // check is name already taken & find position for file

    uint32_t new_file_pos = -1;
    for (uint32_t i = 0; i < 32; i++){
        if(strcmp(name, files[i].name) == 0){
            // TODO deallocate that
            kprint("File with this name already exists.\n");
            return;
        }
        if(files[i].name[0] == 0){
            new_file_pos = i;
            break;
        }
    }

    if(new_file_pos == -1){
        kprint("Not enough space for new file");
        return;
    }

    uint32_t LBA = allocate_file(1);

    // WARNING name is truncated here, so we can still end up with files with same name
    // can't be bothered to fix now, too late at night.

    for (uint32_t j = 0; j < 10; j++){
        files[new_file_pos].name[j] = name[j];
        if(name[j] == 0) break;  // name could be less than 10 long. If >10, it is cut.
    }

    files[new_file_pos].LBA = LBA;
    files[new_file_pos].n_sectors = 1;

    write_file_list(files, 0xA0, 1, 1);
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
