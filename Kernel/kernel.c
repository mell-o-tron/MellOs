/*********************
* TEXT MODE: 0xB8000 *
* GR.  MODE: 0xA000  *
*********************/

//IMPLEMENT STACK SMASHING PROTECTOR!!!!


#include "../Utils/Typedefs.h"
#include "../Misc/colors.h"
#include "../Drivers/VGA_Text.h"
#include "../Utils/Conversions.h"
#include "../Utils/string.h"
#include "../Memory/mem.h"
#include "../Memory/cmos_memsize.h"
#include "../Memory/Paging/paging.h"
#include "../CPU/Interrupts/idt.h"
#include "../CPU/Interrupts/isr.h"
#include "../CPU/Interrupts/irq.h"
#include "../CPU/Timer/timer.h"
#include "../CPU/GDT/gdt.h"
#include "../Drivers/Keyboard.h"
#include "../Drivers/Floppy.h"
#include "../Drivers/port_io.h"
#include "../Utils/dataStructures.h"
#include "../Shell/shell.h"

#include "../Drivers/Disk.h"
#include "../Drivers/PCSpeaker.h"
#include "../Processes/flat_binary_loader.h"
#include "../Utils/bitmap.h"
#include "../Utils/assert.h"
#include "../Memory/dynamic_mem.h"
#include "../FileSystem/placeholder_fs.h"


volatile int curMode = 0;                            // Modes:    0: dummy text, 10: shell


extern const char Fool[];                           // Test included binaries
extern const char KPArt[];
extern const unsigned short MemSize;                // General purpose thing with misleading name
extern int curColor;
char ker_tty[4000];



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

// TODO allocate these in the heap (using the new KMALLOC)
unsigned int page_directory[1024] __attribute__((aligned(4096)));
unsigned int first_page_table[1024] __attribute__((aligned(4096)));
unsigned int second_page_table[1024] __attribute__((aligned(4096)));

PD_FLAGS page_directory_flags = PD_PRESENT + PD_READWRITE;
PT_FLAGS first_page_table_flags = PT_PRESENT + PT_READWRITE;


extern  void main(){
    //asm volatile("1: jmp 1b");        // "entry breakpoint" (debug)
    

    init_paging(page_directory, first_page_table);
    
    add_page(page_directory, second_page_table, 1, 0x1000000, page_directory_flags, first_page_table_flags);
    
    
    GDT_Init();
    idt_install();
    isrs_install();
    irq_install();
    asm volatile ("sti");
    timer_install();
    initializeMem();


    clear_tty(DEFAULT_COLOR, ker_tty);
    display_tty(ker_tty);
    clear_tty(DEFAULT_COLOR, ker_tty);
    display_tty_line(ker_tty, 1);
    int curColor = DEFAULT_COLOR;
    SetCursorPosRaw(0);
    kprintCol(Fool, DEFAULT_COLOR);
    kprintCol("\n", DEFAULT_COLOR);
    kb_install();
    
    
    ///////////////////////////////////////////////////////////
    
    // TESTING LAND
    
                                                // dynamic memory allocation setup test
    set_alloc_bitmap((bitmap_t) 0x400000, 10000);
    set_dynamic_mem_loc (0x400000 + 10000/2);
    
    set_bitmap(get_allocation_bitmap(), 8);     // first fit algo check
    
    void * code_loc = kmalloc(10);              // kmalloc test
    
    if (code_loc == NULL){                      // null check
        kprintCol("null", DEFAULT_COLOR);
        
        for (;;){;}
    }
    
    
//     SetCursorPosRaw(80 * 17);
    
    
    kprint("0x");                              // kmalloc result test
    kprint(toString((int) code_loc , 16));    
    kprint("    ");
    
    code_loc = krealloc(code_loc, 10, 15);
    
    kprint("0x");                              // kmalloc result test
    kprint(toString((int) code_loc , 16));    
    kprint("\n");
    
    void * print_loc = (void*) 0x1000;
    
                                                // binary loading test
    load_flat_binary_at(syscall_test, 7, code_loc);
    
//     SetCursorPosRaw(80 * 18);
    
    run_flat_binary(code_loc);                  // running binary (result: syscall)
    
    kprint(toString(kfree(code_loc, 15), 10));  // kfree test
    
    
    kprint("\n");
    
    bitmap_t allocation_bitmap = get_allocation_bitmap();

    /*
                                                // print first 50 bits of the allocation bitmap
    for (int i = 0; i < 50; i++){
        if (get_bitmap (allocation_bitmap, i) == 1)
            kprintChar('1', 0);
        else
            kprintChar('0', 0);
    }*/
    
    
    ///////////////////////////////////////////////////////////
    
    // INACTIVE TESTING
    
    ///////////////////////////////////////////////////////////
    
                                                // FS test
    
    
//     uint16_t addr_w [256];          // TESTING zero out disk at beginning
//     uint16_t addr_r [256];
//     
//     for (int i = 0 ; i < 256; i++){
//         addr_w[i] = 0;
//         addr_r[i] = 0;
//     }
//     
//     
//     LBA28_write_sector(0xA0, 1, 1, addr_w);
//     
//     initial_file(0xA0, "FIRST", 0, 0, 3, 1);
//     
//     for(;;);
    
//     new_file(0xA0, "PALLE", 0, 0, 3, 1);
    
//     int *file_num = 0;
// 
//     file_mmd **files = get_root_files(0xA0, &file_num);
// 
//     if(files == NULL) {
//         kprint("files is null");
//         for(;;);
//     }
//     
//     if(files[0] == NULL){
//         kprint("files[0] is null");
//         for(;;);
//     }
//     
//     
//      
//     clear_tty(DEFAULT_COLOR, ker_tty);
//     display_tty(ker_tty);
//     
//     SetCursorPosRaw(0);
//     
//     kprint("\nnumber of files:");
//     kprint(toString(*file_num, 10));
//     kprint("\n");
//     list_files (files, *file_num);
    
    ///////////////////////////////////////////////////////////
    
                                                // disk tests
    
    /*uint16_t addr_w [256];
    uint16_t addr_r [256];
    
    for (int i = 0 ; i < 256; i++){
        addr_w[i] = 0;
        addr_r[i] = 0;
    }
    
    
//     for (int i = 0 ; i < 10; i++){
//         addr_w[i] = ('a' + i);              // & 0xff ~ only read lowest 8 bits
//     }
    
    LBA28_write_sector(0xA0, 1, 1, addr_w);
    
    LBA28_read_sector(0xA0, 1, 1, addr_r);
    
    for (int i = 0 ; i < 256; i++){
        kprintChar(addr_r[i] & 0xFF, 0);              // & 0xff ~ only read lowest 8 bits
        kprintChar((addr_r[i] >> 8) & 0xFF, 0);
        display_tty(ker_tty);
    }
        */
    
    return;
}
