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

volatile int curMode = 0;                            // Modes:    0: dummy text, 10: shell


extern const char Fool[];                // Test included binaries
extern const char KPArt[];
extern const unsigned short MemSize;    // Approximate value of extended memory (under 4 GB)
extern int curColor;
char ker_tty[4000];

// This function has to be self contained - no dependencies to the rest of the kernel!
extern  void kpanic(struct regs *r){
    
    #define ERRCOL 0x47 // Lightgrey on Lightred
    #define VGAMEM (unsigned char*)0xB8000;
    char keyboard_stack[256];
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

// TODO allocate these in the heap in a reproducible manner
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
    
    
    void * code_loc = (void*) 0x400000;
    void * print_loc = (void*) 0x1000;
    
    load_flat_binary_at(syscall_test, 7, code_loc);
    run_flat_binary(code_loc);

    
    return;
}
