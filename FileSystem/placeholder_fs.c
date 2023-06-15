#include "../Drivers/Disk.h"
#include "../Memory/dynamic_mem.h"
#include "placeholder_fs.h"
#include "../Utils/Typedefs.h"
#include "../Utils/Conversions.h"
#include "../Utils/string.h"
#include "../Misc/colors.h"
#include "../Drivers/VGA_Text.h"


// The first sector will contain a magic number and a bitmap of sectors
// The second few sectors contain the meta-meta-data of the files in root       TBD     #sec / dir
// These MMD consist of the file number, the type (0 file, 1 dir), the permissions, the first sector and the number of sectors assigned.

#define SECTORS_PER_DIRECTORY 1


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
    for (int i = 0; i < 2; i++) {
        total_size += strlen(file_list[i]->file_name) + 1;  // Include the null terminator
        total_size += sizeof(char) * 3;  // type, permissions, first_sector
        total_size += sizeof(int);  // num_sectors
    }
    
    unsigned char* byte_array = (unsigned char*)kmalloc(total_size);
    // Split file_num into four bytes and copy to byte array
    *(byte_array++) = (char)(file_num >> 24);
    *(byte_array++) = (char)(file_num >> 16);
    *(byte_array++) = (char)(file_num >> 8);
    *(byte_array++) = (char)(file_num);
    
    // Write the struct values to the byte array
    unsigned char* current_byte = byte_array;
    for (int i = 0; i < 2; i++) {
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
    
    // Iterate over the byte array to reconstruct struct instances
    const unsigned char* current_byte = byte_array;
    for (size_t i = 0; i < file_num; i++) {
        // Allocate memory for the struct instance
        file_list[i] = (file_mmd*)kmalloc(sizeof(file_mmd));
        
        // Copy file_name string from byte array
        size_t file_name_length = strlen((const char*)current_byte) + 1;
        file_list[i]->file_name = (char*)kmalloc(file_name_length);
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
    
    /* ATTENTION the disk r/w functions are a bit weird and buggy, modify them if needed */
    
    /* TODO */

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

void new_file (int disk, char* name, char type, char permissions, int first_sector, int num_sectors){

    /* should get file list, append a file, and rewrite the whole thing to disk. I know it's a bit silly, but it's called "placeholder_fs" for a reason lol*/
    /* TODO */
    
}
