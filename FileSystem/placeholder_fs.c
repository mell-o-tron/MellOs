#include "../Drivers/Disk.h"
#include "../Memory/dynamic_mem.h"
#include "placeholder_fs.h"
#include "../Utils/Typedefs.h"
#include "../Utils/Conversions.h"
#include "../Misc/colors.h"
#include "../Drivers/VGA_Text.h"


// The first sector contains a magic number and a bitmap of sectors
// The second few sectors contain the meta-meta-data of the files in root       TBD     #sec / dir
// These MMD consist of the file number, the type (0 file, 1 dir), the permissions, the first sector and the number of sectors assigned.

#define SECTORS_PER_DIRECTORY 1


char byte_from_16arr (uint16_t* array, int i){
    int j = i / 2;
    int r = i % 2;
    return (array[j] >> (8 * r)) & 0xff;
}



int recognise_PFS(int disk) {

    uint16_t addr_r [256];
    
    LBA28_read_sector(disk, 1, 1, addr_r);
    
    if(addr_r[0] = 0x1234)
        return 1;
    else
        return 0;
}


file_mmd ** get_root_files (int disk, int** file_num){
    
    file_mmd **result = kmalloc(sizeof(file_mmd*) * 10);
    
    if(result == NULL) {
            kprint("malloc failed: get_root_files, result\n");
            for(;;);
    }
    
    int nfiles = 10;
    
    uint16_t addr_r [256 * SECTORS_PER_DIRECTORY];
    
    
    LBA28_read_sector(disk, 1, SECTORS_PER_DIRECTORY, addr_r);          // TESTING TODO change this to sector 2
    
    kprint("\n");
    for (int i = 0; i < 15; i++){
        kprint(toString(byte_from_16arr(addr_r, i), 16));
        kprint(" ");
    }
    kprint("\n");
    
    int i = 0;
    int struct_num = 0;
    
    
    
    
    while(i < 512 * SECTORS_PER_DIRECTORY){
        
        if (byte_from_16arr(addr_r, i) == 0 || 
            byte_from_16arr(addr_r, i) == '\n'){
//             kprint("done");
            break;
        }
//         else{
//             kprint(" char = ");
//             kprint(toString(byte_from_16arr(addr_r, i), 16));
//         }
        
        char* file_name = kmalloc(17);
        
        if(file_name == NULL) {
            kprint("malloc failed: get_root_files, file name\n");
            for(;;);
        }
        
        for (int c = 0; c < 17; c++) file_name[c] = 0;
        
        int k = 0; 
        for (;k < 16; k++){
            char c = byte_from_16arr(addr_r, k + i);
            if (c == 0 || c == '\n') break;
            file_name[k] = c;
        }
//         kprint("\n name len = ");
//         kprint(toString(k, 10));
//         
//         kprint("\n name = ");
//         kprint(file_name);
//         kprint("\n");
//         

        
        i+=k;
        
           
        
        
        char type           = byte_from_16arr(addr_r, i+1);
        char permissions    = byte_from_16arr(addr_r, i+2);
        int first_sector    = byte_from_16arr(addr_r, i + 3) | (byte_from_16arr(addr_r, i+4) << 8) | (byte_from_16arr(addr_r, i+5) << 16) | (byte_from_16arr(addr_r, i+6) << 24);
        int num_sectors     = byte_from_16arr(addr_r, i + 7) | (byte_from_16arr(addr_r, i+8) << 8) | (byte_from_16arr(addr_r, i+9) << 16) | (byte_from_16arr(addr_r, i+10) << 24);
        
//         kprint("\n type = ");
//         kprint(toString(type, 16));
//         kprint("  p = "); 
//         kprint(toString(permissions, 16));
//         kprint("  fs = "); 
//         kprint(toString(first_sector, 16));
//         kprint("  ns = "); 
//         kprint(toString(num_sectors, 16));
//         kprint("  "); 

        
        i += 11;
        
        
        
        
        if (struct_num > nfiles) {
            krealloc(result, nfiles * sizeof(file_mmd*), 2 * nfiles * sizeof(file_mmd*));
            nfiles *= 2;
        }
        
        file_mmd * new_mmd = kmalloc (sizeof(file_mmd));
        
        if(new_mmd == NULL) {
            kprint("malloc failed: get_root_files, new_mmd\n");
            for(;;);
        }
        
        
        new_mmd -> file_name = file_name;
        new_mmd -> type = type;
        new_mmd -> permissions = permissions;
        new_mmd -> first_sector = first_sector;
        new_mmd -> num_sectors = num_sectors;
        
        result[struct_num] = new_mmd;
        struct_num++;
        
    }
    
    
    krealloc(result, nfiles * sizeof(file_mmd*), struct_num * sizeof(file_mmd*));
    if(result == NULL) {
        kprint("realloc failed: get_root_files, result\n");
        for(;;);
    }
    
    
    *(*file_num) = struct_num;
    return result;

}


void write_mmd_list (int disk, file_mmd ** mmd, int number){
    uint16_t addr_w [128 * SECTORS_PER_DIRECTORY];
    
    for (int i = 0; i < 128 * SECTORS_PER_DIRECTORY; i++){
        addr_w[i] = 0;
    }
    
    int j = 0;
    int j_side = 0;
    
    for (int i = 0; i < number; i++){
        char* file_name  = mmd[i] -> file_name;
        char type        = mmd[i] -> type;
        char permissions = mmd[i] -> permissions;
        int first_sector = mmd[i] -> first_sector;
        int num_sectors  = mmd[i] -> num_sectors;
        
        int k = 0;
        while(file_name[k] != 0){
            
            addr_w[j] = addr_w[j] | (file_name[k] << (8 * j_side));
            
            if (j_side == 1) j++;
            j_side = (j_side + 1) % 2;
            k++;
        }
        
        if (j_side == 1) j++;
        j_side = (j_side + 1) % 2;
        
        addr_w[j] = addr_w[j] | (type << (8 * j_side));
        
        if (j_side == 1) j++;
        j_side = (j_side + 1) % 2;
        
        addr_w[j] = addr_w[j] | (permissions << (8 * j_side));
        if (j_side == 1) j++;
        j_side = (j_side + 1) % 2;
        
        for (int l = 0; l < 4; l++){
            addr_w[j] = addr_w[j] | ( ((first_sector >> 8 * l) & 0xFF) << (8 * j_side));
            if (j_side == 1) j++;
            j_side = (j_side + 1) % 2;
        }
        
        for (int l = 0; l < 4; l++){
            addr_w[j] = addr_w[j] | ( ((num_sectors >> 8 * l) & 0xFF) << (8 * j_side));
            if (j_side == 1) j++;
            j_side = (j_side + 1) % 2;
        }
        
        
    }
    
   kprint("\n");
     
     for (int i = 0; i < 15; i++){
         kprint(toString(byte_from_16arr(addr_w, i), 16));
         kprint(" ");
     }
    
    
    LBA28_write_sector(disk, 1, SECTORS_PER_DIRECTORY, addr_w);
    
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

// FIXME doestn't work for some reason that maybe has to do with the disk drivers

void new_file (int disk, char* name, char type, char permissions, int first_sector, int num_sectors){
    
    int *num_files;
    
    file_mmd** files = get_root_files(disk, &num_files);
    
    if (*num_files == 0)
        files = kmalloc(sizeof(file_mmd*));
    else
        krealloc(files, *num_files * sizeof(file_mmd*), (*num_files+1) * sizeof(file_mmd*));
    
    file_mmd * new_mmd = kmalloc (sizeof(file_mmd));

    new_mmd -> file_name = name;
    new_mmd -> type = type;
    new_mmd -> permissions = permissions;
    new_mmd -> first_sector = first_sector;
    new_mmd -> num_sectors = num_sectors;
    
    files[*num_files] = new_mmd;

    write_mmd_list(disk, files, *num_files + 1);
    
    for (int i = 0; i < *num_files + 1; i++){
        kfree (files[i], sizeof(file_mmd));
    }
    kfree (files, (*num_files + 1 ) * sizeof(file_mmd *));
    
}
