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


// WARNING this implementation is WIP, and does not work yet. TODO debug it thoroughly


// The first sector will contain a magic number and a bitmap of sectors
// The second few sectors contain the meta-meta-data of the files in root       TBD     #sec / dir
// These MMD consist of the file number, the type (0 file, 1 dir), the permissions, the first sector and the number of sectors assigned.

#define SECTORS_PER_DIRECTORY 1
extern char ker_tty[4000];

void write_file_to_byte_array(const file_mmd* file, unsigned char* byte_array) {
    // Copy file_name string to byte array
    strcpy((char*)byte_array, file->file_name);
    byte_array += strlen(file->file_name) + 1;  // Move the byte pointer to next position
    
    // Copy type, permissions, first_sector as char values to byte array
    *(byte_array++) = file->type;
    *(byte_array++) = file->permissions;
    *(byte_array++) = (char)(file->first_sector);
    
    // Split num_sectors into four bytes and copy to byte array
    *(byte_array++) = (char)(file->num_sectors >> 24);
    *(byte_array++) = (char)(file->num_sectors >> 16);
    *(byte_array++) = (char)(file->num_sectors >> 8);
    *(byte_array++) = (char)(file->num_sectors);
}

unsigned char* write_files_to_byte_array(file_mmd** file_list, int file_num, int * total_size_return){
    
    // Calculate the total size required for the byte array (4 = header size)
    int total_size = 4;
    for (int i = 0; i < file_num; i++) {
//         kprint("file name length: ");
//         kprint(toString(strlen(file_list[i]->file_name), 10));
//         kprint("\n");
        total_size += strlen(file_list[i]->file_name) + 1;  // Include the null terminator
        total_size += sizeof(char) * 3;  // type, permissions, first_sector
        total_size += sizeof(int);  // num_sectors
    }
    
    unsigned char* byte_array = (unsigned char*)kmalloc(total_size);
    
    if (byte_array == NULL) return NULL;
    
    // Split file_num into four bytes and copy to byte array
    *(byte_array++) = (char)(file_num >> 24);
    *(byte_array++) = (char)(file_num >> 16);
    *(byte_array++) = (char)(file_num >> 8);
    *(byte_array++) = (char)(file_num);
    
    // Write the struct values to the byte array
    unsigned char* current_byte = byte_array;
    for (int i = 0; i < file_num; i++) {
        write_file_to_byte_array(file_list[i], current_byte);
        current_byte += strlen(file_list[i]->file_name) + 1 + sizeof(char) * 3 + sizeof(int);
    }
    *total_size_return = total_size;
    
    return byte_array - 4; // Subtract header length
}

file_mmd** get_files_from_byte_array(const unsigned char* byte_array, int* file_num_return) {
    
    int file_num = 0;
    file_num |= *(byte_array++) << 24;
    file_num |= *(byte_array++) << 16;
    file_num |= *(byte_array++) << 8;
    file_num |= *(byte_array++);
    
    *file_num_return = file_num;
    
    // Allocate memory for the struct pointers
    file_mmd** file_list = (file_mmd**)kmalloc(file_num * sizeof(file_mmd*));
    
    if (file_list == NULL) return NULL;
    
    // Iterate over the byte array to reconstruct struct instances
    const unsigned char* current_byte = byte_array;
    for (size_t i = 0; i < file_num; i++) {
        // Allocate memory for the struct instance
        file_list[i] = (file_mmd*)kmalloc(sizeof(file_mmd));
        
        if (file_list[i] == NULL) return NULL;
        
        // Copy file_name string from byte array
        size_t file_name_length = strlen((const char*)current_byte) + 1;
        file_list[i]->file_name = (char*)kmalloc(file_name_length);
        if (file_list[i]->file_name == NULL) return NULL;
            
        strcpy(file_list[i]->file_name, (const char*)current_byte);
        
        current_byte += file_name_length;  // Move the byte pointer to next position
        
        // Copy type, permissions, first_sector as char values from byte array
        file_list[i]->type = *(current_byte++);
        file_list[i]->permissions = *(current_byte++);
        file_list[i]->first_sector = (int)(*current_byte++);
        
        // Reconstruct num_sectors from four bytes in the byte array
        file_list[i]->num_sectors = 0;
        file_list[i]->num_sectors |= (int)(*current_byte++) << 24;
        file_list[i]->num_sectors |= (int)(*current_byte++) << 16;
        file_list[i]->num_sectors |= (int)(*current_byte++) << 8;
        file_list[i]->num_sectors |= (int)(*current_byte++);
    }
    
    return file_list;
}

/* let's not use this in the beginning (indi bininging), ignore the FS recognition part for now*/
int recognise_PFS(int disk) {

    uint16_t addr_r [256];
    
    LBA28_read_sector(disk, 1, 1, addr_r);
    
    if(addr_r[0] = 0x1234)
        return 1;
    else
        return 0;
}


file_mmd ** get_root_files (int disk, int** file_num){
    
    /* should simply read the files from disk and send them through get_files_from_byte_array, and set filenum to the file number */
    uint16_t * addr = kmalloc(SECTORS_PER_DIRECTORY * 256 * sizeof(uint16_t));
    
    if (!addr) return NULL;
    
    unsigned char * addr_8 = kmalloc(SECTORS_PER_DIRECTORY * 256);
    
    if(!addr_8) return NULL;
    
    LBA28_read_sector(disk, 1, SECTORS_PER_DIRECTORY, addr);
    
    for (int i = 0 ; i < 256; i++)
        addr_8[i] = addr[i] & 0xFF, 0;              // & 0xff ~ only read lowest 8 bits
    
    int * file_num_return = 0;
    
    kprint("bits follow: ");
    /*
    for (int i = 0; i < 10; i++){
        kprint("0x");
        kprint(toString(addr_8[i], 16));
        kprint(" ");
    }*/
    
    kprint("  ");
    
    file_mmd ** result = get_files_from_byte_array(addr_8, file_num_return);
    
    if (!result) return NULL;
    
    kprint(toString(*file_num_return, 10));
    
    file_num = &file_num_return;
    
    return result;
}


void list_files (file_mmd ** mmd, int number){
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

maybe_void new_file (int disk, char* name, char type, char permissions, int first_sector, int num_sectors, bitmap_t disk_bitmap, int bitmap_size){

    /* should get file list, append a file, and rewrite the whole thing to disk. I know it's a bit silly, but it's called "placeholder_fs" for a reason lol*/
    int *file_num = 0;
    
    file_mmd * new = kmalloc(sizeof(file_mmd));
    
    if (!new) fail(1);
    
    new -> file_name = name;
    new -> type = type;
    new -> permissions = permissions;
    new -> first_sector = first_sector;
    new -> num_sectors = num_sectors;
    
    file_mmd ** old_files = get_root_files (disk, &file_num);
    
    if (!old_files) fail(2);
//     file_mmd ** new_files = krealloc (old_files, *file_num * sizeof(file_mmd *), (*file_num + 1) * sizeof(file_mmd *));

    file_mmd ** new_files = kmalloc(sizeof(file_mmd *) * (*file_num + 1));
    
    if (!new_files) fail(1);
    
    for (int i = 0; i < *file_num ; i++) new_files[i] = old_files[i];
    
    kfree(old_files, sizeof(file_mmd *) * (*file_num));
    
    kprint(toString(*file_num, 10));
    
    new_files [*file_num] = new;
    
     
    int total_size;
    unsigned char * new_file_array = write_files_to_byte_array (new_files, *file_num + 1, &total_size);
    
    if(!new_file_array) fail(2);
    
    uint16_t * array_16 = kmalloc(total_size * sizeof(uint16_t));
    
    if (!array_16) fail(1);
    
    for (int i = 0 ; i < total_size; i++)
        array_16[i] = (uint16_t)new_file_array[i];
    
    LBA28_write_sector(disk, 1, SECTORS_PER_DIRECTORY, array_16);
    
    if(first_sector + num_sectors > bitmap_size) fail(3);
    
    for(int i = first_sector; i < first_sector + num_sectors; i++)
        set_bitmap(disk_bitmap, i);
    
    succeed;
}


maybe_void initial_file (int disk, char* name, char type, char permissions, int first_sector, int num_sectors, bitmap_t disk_bitmap, int bitmap_size){
    int *file_num = 0;
    
    file_mmd * new = kmalloc(sizeof(file_mmd));
    if (!new) fail(1);
    new -> file_name = name;
    new -> type = type;
    new -> permissions = permissions;
    new -> first_sector = first_sector;
    new -> num_sectors = num_sectors;
    
    file_mmd ** new_files = kmalloc(sizeof(file_mmd *));
    if (!new_files) fail(1);
    
    new_files [0] = new;
    int total_size;
    
    unsigned char * new_file_array = write_files_to_byte_array (new_files, 1, &total_size);
    if(!new_file_array) fail(2);
    
//     kprint(toString(total_size, 10));

    uint16_t * array_16 = kmalloc(total_size * sizeof(uint16_t));
    if (!array_16) fail(1);
    
    for (int i = 0 ; i < total_size; i++)
        array_16[i] = (uint16_t)new_file_array[i];
    
    LBA28_write_sector(disk, 1, SECTORS_PER_DIRECTORY, array_16);
    
    if(first_sector + num_sectors > bitmap_size) fail(3);
    
    for(int i = first_sector; i < first_sector + num_sectors; i++)
        set_bitmap(disk_bitmap, i);
    
    succeed;
}

maybe_int get_free_sector(int size, bitmap_t disk_bitmap, int bitmap_size){
    int consecutive = 0;
    int result = 1 + SECTORS_PER_DIRECTORY;     // sectors start from 1, + root directory space.
    for (int i = 1+ SECTORS_PER_DIRECTORY; i < bitmap_size; i++){
        if (get_bitmap(disk_bitmap, i) == 0)
            consecutive ++;
        else{
            consecutive = 0;
            result = i+1;
        }
        if (consecutive >= size) return result;
    }
    fail(1);
}

maybe_void new_file_alloc(int disk, char* name, char type, char permissions, int num_sectors, bitmap_t disk_bitmap, int bitmap_size){
    int position = get_free_sector(num_sectors, disk_bitmap, bitmap_size);
    if(position < 0) fail(1);
    return msg_on_fail(
        new_file (disk, name, type, permissions, position, num_sectors, disk_bitmap, bitmap_size),
        "new file creation failed (called from new_file_alloc)"
    );
}

maybe_void initial_file_alloc(int disk, char* name, char type, char permissions, int num_sectors, bitmap_t disk_bitmap, int bitmap_size){
    int position = get_free_sector(num_sectors, disk_bitmap, bitmap_size);
    if(position < 0) fail(1);
    return msg_on_fail(
        initial_file (disk, name, type, permissions, position, num_sectors, disk_bitmap, bitmap_size),
        "new file creation failed (called from initial_file_alloc)"
    );
}


maybe_void write_file (int disk, file_mmd* file, uint16_t* contents, int content_size){
    if (content_size != file -> num_sectors) fail(1);      // size measured in sectors
    
    LBA28_write_sector(disk, file -> first_sector, content_size, contents);
    succeed;
}

maybe_void read_file (int disk, file_mmd* file, uint16_t* contents, int content_size){
    if (content_size != file -> num_sectors) fail(1);      // size measured in sectors
    
    LBA28_read_sector(disk, file -> first_sector, content_size, contents);
    
    //TODO check if read went ok
    
    succeed;
    
}
