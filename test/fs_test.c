#include "../file_system/file_system.h"
#include "../utils/string.h"
#ifdef VGA_VESA
#include "../drivers/vesa/vesa.h"
#include "../drivers/vesa/vesa_text.h"
#include "../drivers/mouse.h"
#else
#include "../drivers/vga_text.h"
#endif

int create_file_test () {
    remove_file("test");
    new_file("test", 1);
    write_string_to_file("this is the night mail crossing the border", "test");
    char* s = read_string_from_file("test");
    int res = strcmp(s, "this is the night mail crossing the border");

    // TODO should free s here

    remove_file("test");
    return res == 0 ? 0 : 1;
}


    /*
    // this clears the disk, remove it to have persistence
    kprint("Erasing virtual disk (debug)...");
    prepare_disk_for_fs(32);
  
    char* tmp = kmalloc(512);
    
    for (uint32_t i = 0; i < 512; i++)
        tmp[i] = 0;
    
    // test program that prints to screen
    
    tmp[0] = 0xB8;
    tmp[1] = 0x04;
    tmp[2] = 0x00;
    tmp[3] = 0x00;
    tmp[4] = 0x00;
    tmp[5] = 0xBB;
    tmp[6] = 0x01;
    tmp[7] = 0x00;
    tmp[8] = 0x00;
    tmp[9] = 0x00;
    tmp[10] = 0xB9;
    tmp[11] = 0x17;
    tmp[12] = 0x00;
    tmp[13] = 0x70;
    tmp[14] = 0x00;
    tmp[15] = 0xBA;
    tmp[16] = 0x10;
    tmp[17] = 0x00;
    tmp[18] = 0x00;
    tmp[19] = 0x00;
    tmp[20] = 0xCD;
    tmp[21] = 0x80;
    tmp[22] = 0xC3;
    tmp[23] = 0x48;
    tmp[24] = 0x65;
    tmp[25] = 0x6C;
    tmp[26] = 0x6C;
    tmp[27] = 0x6F;
    tmp[28] = 0x20;
    tmp[29] = 0x77;
    tmp[30] = 0x65;
    tmp[31] = 0x69;
    tmp[32] = 0x72;
    tmp[33] = 0x64;
    tmp[34] = 0x6F;
    tmp[35] = 0x21;
    tmp[36] = 0x0A;
    tmp[37] = 0x0D;
    tmp[38] = 0x00;

    new_file("print.bin", 1);
    write_string_to_file(tmp, "print.bin");
    
    
     for (uint32_t i = 0; i < 512; i++)
        tmp[i] = 0;
    
    // test program that writes to file "banana" with file descriptor (=LBA) 5
    
    tmp[0] = 0xB8;
    tmp[1] = 0x04;
    tmp[2] = 0x00;
    tmp[3] = 0x00;
    tmp[4] = 0x00;
    tmp[5] = 0xBB;
    tmp[6] = 0x05;
    tmp[7] = 0x00;
    tmp[8] = 0x00;
    tmp[9] = 0x00;
    tmp[10] = 0xB9;
    tmp[11] = 0x17;
    tmp[12] = 0x00;
    tmp[13] = 0x70;
    tmp[14] = 0x00;
    tmp[15] = 0xBA;
    tmp[16] = 0x15;
    tmp[17] = 0x00;
    tmp[18] = 0x00;
    tmp[19] = 0x00;
    tmp[20] = 0xCD;
    tmp[21] = 0x80;
    tmp[22] = 0xC3;
    tmp[23] = 0x48;
    tmp[24] = 0x65;
    tmp[25] = 0x6C;
    tmp[26] = 0x6C;
    tmp[27] = 0x6F;
    tmp[28] = 0x20;
    tmp[29] = 0x6D;
    tmp[30] = 0x69;
    tmp[31] = 0x73;
    tmp[32] = 0x74;
    tmp[33] = 0x65;
    tmp[34] = 0x72;
    tmp[35] = 0x20;
    tmp[36] = 0x66;
    tmp[37] = 0x69;
    tmp[38] = 0x6C;
    tmp[39] = 0x65;
    tmp[40] = 0x21;
    tmp[41] = 0x0A;
    tmp[42] = 0x0D;
    tmp[43] = 0x00;

    new_file("write.bin", 1);
    write_string_to_file(tmp, "write.bin");
    
    new_file("banana", 1);
    */


int run_all_fs_tests () {
    return create_file_test() /* + other tests ....  */;
}