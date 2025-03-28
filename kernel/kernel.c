/*********************
* TEXT MODE: 0xB8000 *
* GR.  MODE: 0xA000  *
*********************/


#include "../utils/typedefs.h"
// #include "../utils/error_handling.h"                // read docs!
#include "../misc/colours.h"
#include "../drivers/vga_text.h"
#include "../utils/conversions.h"
// #include "../utils/string.h"
#include "../cpu/interrupts/idt.h"
#include "../cpu/interrupts/isr.h"
#include "../cpu/interrupts/irq.h"
#include "../cpu/timer/timer.h"
#include "../cpu/gdt/gdt.h"
#include "../drivers/keyboard.h"
#include "../drivers/port_io.h"
#include "../memory/paging/paging.h"
#include "../memory/dynamic_mem.h"
#include "../data_structures/allocator.h"
// #include "../utils/assert.h"

// PROCESSES
#include "../processes/processes.h"
// #include "../processes/stack_utils.h"

// MATH
// #include "../utils/math.h"

// DISK
// #include "../drivers/disk.h"
#include "../disk_interface/diskinterface.h"

// SHELL
#include "../shell/shell.h"

// FILE SYSTEM
#include "../file_system/file_system.h"

// TEXT_EDITOR
#include "../text_editor/text_editor.h"


extern const char KPArt[];
extern const char Fool[];

bool keyboard_enabled = false; // maybe put this in some "state" struct?


// This function has to be self contained - no dependencies to the rest of the kernel!
extern  void kpanic(struct regs *r){
    
    #define ERRCOL 0x47 // Lightgrey on Lightred
    #define VGAMEM (unsigned char*)0xB8000;

    char panicscreen[4000];
    const char* components[] = {
        KPArt,
        "Exception message: ",
        exception_messages[r->int_no],
    };
    int psidx = 0; //Index to access panicscreen
    int idx = 0;

    for(int x = 0; x < sizeof(components)/sizeof(char*); x++){
        idx = 0;
        while(components[x][idx] != 0){
            if(components[x][idx] == '\n'){
                do{
                    panicscreen[psidx] = ' ';
                    psidx++;
                } while((psidx+1) % 80 != 0);
            } else panicscreen[psidx] = components[x][idx];
            psidx++;
            idx++;
        }
    }

    unsigned char *write = VGAMEM;

    for(int i = 0; i < 4000; i++){
        *write++ = panicscreen[i];
        *write++ = ERRCOL;
    }

    // Disables the flashing cursor because that's annoying imo
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);

    for(;;);
}

unsigned int page_directory[1024]       __attribute__((aligned(4096)));
unsigned int first_page_table[1024]     __attribute__((aligned(4096)));
unsigned int second_page_table[1024]    __attribute__((aligned(4096)));

PD_FLAGS page_directory_flags   = PD_PRESENT | PD_READWRITE;
PT_FLAGS first_page_table_flags = PT_PRESENT | PT_READWRITE;

extern int top_of_stack ();

void test_task(){
    kprint_col("test task print\n", DEFAULT_COLOUR);

    return;
}

void task_1(){
    kprint("Hello there!\n");
    for (;;){
        try_to_terminate();
    }
}

void task_2(){
    kprint("BOIA DE\n");
    for (;;){
        try_to_relinquish();
    }
}

extern void main(){
    
    // identity-maps 0x0 to 4MB (i.e. 0x400000 - 1)
    init_paging(page_directory, first_page_table);

    // maps 4MB to 8MB (0x400000 to 0x800000 - 1) -> 16 MB to 20 MB (0x1000000 to 0x1400000 - 1)
    add_page(page_directory, second_page_table, 1, 0x1000000, first_page_table_flags, page_directory_flags);

    gdt_init();
    idt_install();
    isrs_install();
    irq_install();
    asm volatile ("sti");
    timer_install();
    clear_screen_col(DEFAULT_COLOUR);
    set_cursor_pos_raw(0);
    
    allocator_t allocator;
    assign_kmallocator(&allocator);
    
    set_kmalloc_bitmap((bitmap_t) 0x400000, 100000);   // dynamic memory allocation setup test
    set_dynamic_mem_loc ((void*)0x400000 + 100000/2);

    kb_install();

    
    void * code_loc = (void*) kmalloc(10);                  // kmalloc test

    if (code_loc == NULL){                          // null check
        kprint_col("KMALLOC TEST FAILED!!", DEFAULT_COLOUR);

        for (;;){;}
    }

    // this clears the disk, remove it to have persistence
    kprint("Erasing virtual disk (debug)...");
    prepare_disk_for_fs(32);
    clear_screen_col(DEFAULT_COLOUR);
  
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
    
    set_cursor_pos_raw(0);
    
    uint8_t* a = read_string_from_disk(0xA0, 1, 1);
    
    
    kprint(Fool);
    
    load_shell();
    
    // init_text_editor("test_file");

    /*
    schedule_process(task_1);
    schedule_process(task_2);

    begin_execution();*/


    return;
}
