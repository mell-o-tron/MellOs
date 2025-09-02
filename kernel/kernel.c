/*********************
* TEXT MODE: 0xB8000 *
* GR.  MODE: 0xA000  *
*********************/


#include "../utils/typedefs.h"
#include "multiboot_tags.h"
// #include "../utils/error_handling.h"                // read docs!
#include "../misc/colours.h"
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
#include "../memory/paging/pat.h"
#include "../memory/dynamic_mem.h"
#include "../data_structures/allocator.h"
#ifdef VGA_VESA
#include "../drivers/vesa/vesa.h"
#include "../drivers/vesa/vesa_text.h"
#include "../drivers/mouse.h"
#include "vell.h"
#include "format.h"
#else
#include "../drivers/vga_text.h"
#endif
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

// TESTS
#include "../test/fs_test.h"
#include "../test/mem_test.h"

#ifdef VGA_VESA
__attribute__((section(".multiboot")))
#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
#define MULTIBOOT_HEADER_FLAGS 0x00000007
const uint32_t multiboot_header[] = {
    MULTIBOOT_HEADER_MAGIC,
    MULTIBOOT_HEADER_FLAGS,
    -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS) & 0xFFFFFFFF,
    // aout kludge (unused)
    0,0,0,0,0,
    // video mode
    0, // Linear graphics please?
    HRES, // Preferred width
    VRES, // Preferred height
    BPP   // Preferred pixel depth
};
#else
__attribute__((section(".multiboot")))
#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
#define MULTIBOOT_HEADER_FLAGS 0x0
const uint32_t multiboot_header[] = {
    MULTIBOOT_HEADER_MAGIC,
    MULTIBOOT_HEADER_FLAGS,
    -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS) & 0xFFFFFFFF
};
#endif

extern const char KPArt[];
extern const char Fool[];

bool keyboard_enabled = false; // maybe put this in some "state" struct?

void khang(){
    for(;;);
}

#pragma GCC push_options
#pragma GCC optimize ("O0")

// This function has to be self contained - no dependencies to the rest of the kernel!
extern  void kpanic(struct regs *r) {

    const char* components[] = {
        KPArt,
        "Exception message: ",
        exception_messages[r->int_no],
    };

#if defined(VGA_VESA) && defined(GRAPHICAL_PANIC)
    char buf[256];
    snprintf(buf, 255, "%s %s %s%i%s", components[1], components[2], "(", r->int_no, ")");

    if (_vell_is_active()) {
        fb_clear_screen_col_VESA(VESA_RED, *vga_fb);
        fb_draw_string(16, 16, buf, VESA_DARK_GREY, 3, 3, *vga_fb);
    } else {
#endif
        #define ERRCOL 0x47 // Lightgrey on Lightred
        #define VGAMEM (unsigned char*)vga_fb;

        char panicscreen[4000];
        
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
#if defined(VGA_VESA) && defined(GRAPHICAL_PANIC)
    }
#endif

    

    // Disables the flashing cursor because that's annoying imo
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);

    for(;;);
}

#pragma GCC pop_options

uint32_t page_directory[1024]       __attribute__((aligned(4096)));
uint32_t first_page_table[1024]     __attribute__((aligned(4096)));
uint32_t second_page_table[1024]    __attribute__((aligned(4096)));
#define NUM_MANY_PAGES (uint32_t)512
uint32_t lots_of_pages[NUM_MANY_PAGES][1024]  __attribute__((aligned(4096)));
#ifdef VGA_VESA
#define NUM_FB_PAGES (uint32_t)2 // FB is 8100 KB (1920x1080x4), so 2 pages are enough
unsigned int framebuffer_pages[NUM_FB_PAGES][1024]     __attribute__((aligned(4096)));
PD_FLAGS framebuffer_page_dflags = PD_PRESENT | PD_READWRITE;
PT_FLAGS framebuffer_page_tflags = PT_PRESENT | PT_READWRITE | PT_WRITECOMBINING;
#endif

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

// char test[0xe749] = {1};
allocator_t allocator;

extern void main(uint32_t multiboot_tags_addr){
        
    // identity-maps 0x0 to 8MB (i.e. 0x800000 - 1)
    init_paging(page_directory, first_page_table, second_page_table);
    
    // Sets up the Page Attribute Table
    setup_pat();

    
    // Maps a few pages for future use. Until we have a page manager, we just have a fixed number of pages
    for(uint32_t i = 0; i < NUM_MANY_PAGES; i++){
        add_page(page_directory, lots_of_pages[i], i + 2, 0x400000 * (i + 2), first_page_table_flags, page_directory_flags);
    }
    

    #ifdef VGA_VESA
    // Map two pages for the framebuffer
    const uint32_t framebuffer_addr = 0x400000 * (2 + NUM_MANY_PAGES); // Addr of the next page that will be added
    for (int i = 0; i < NUM_FB_PAGES; i++){
        add_page(page_directory, framebuffer_pages[i], 2 + NUM_MANY_PAGES + i, 0xFD000000 /*This value should be retrieved from the vbe mode info retrieved during boot*/ + i * 0x400000, framebuffer_page_tflags, framebuffer_page_dflags);
    }
    const uint32_t framebuffer_end = 0x400000 * (2 + NUM_MANY_PAGES + NUM_FB_PAGES);
    #endif
    
    gdt_init();
    idt_install();
    isrs_install();
    irq_install();
    // asm volatile("hlt");
    
    
    asm volatile ("sti");
    timer_install();
    clear_screen_col(DEFAULT_COLOUR);
    set_cursor_pos_raw(0);
    
    //allocator.granularity = 512;
    //assign_kmallocator(&allocator);
    buddy_init((void *)0x800000, 100000000);

    //set_kmalloc_bitmap((bitmap_t) 0x800000, 100000000);   // dynamic memory allocation setup test. Starting position is at 0x800000 as we avoid interfering with the kernel at 0x400000
    #ifdef VGA_VESA
    // set_dynamic_mem_loc ((void*)framebuffer_end);
    set_dynamic_mem_loc ((void*)0x800000 + 100000000/2);
    MultibootTags* multiboot_tags = (MultibootTags*)multiboot_tags_addr;
    Hres = multiboot_tags->framebuffer_width;
    Vres = multiboot_tags->framebuffer_height;
    Pitch = multiboot_tags->framebuffer_pitch / BYTES_PER_PIXEL; // Convert to pixels
    _vesa_framebuffer_init(framebuffer_addr);
    _vesa_text_init();
    mouse_install();
    #else
    set_dynamic_mem_loc ((void*)0x800000 + 100000000/2);
    #endif
    
    
    kb_install();

    kprint("Running fs tests... ");
    int failed_fs_tests = run_all_fs_tests();
    kprint(tostring_inplace(failed_fs_tests, 10));
    kprint(" failed\n");

    kprint("Running mem tests... ");
    int failed_mem_tests = run_all_mem_tests();
    kprint(tostring_inplace(failed_mem_tests, 10));
    kprint(" failed\n");

    kprint("\n\n ENTERING COMMAND MODE...\n");


    sleep(30);

    void * code_loc2 = (void*) kmalloc(10);
    if (code_loc2 == NULL){
        kprint_col("SLAB ALLOC TEST FAILED!!", DEFAULT_COLOUR);

        for (;;){;}
    }
    
    #ifdef VGA_VESA
    kclear_screen();
    #else
    clear_screen_col(DEFAULT_COLOUR);
    #endif

    set_cursor_pos_raw(0);

    kprint(Fool);
    
    load_shell();
    // init_text_editor("test_file");

    /*
    schedule_process(task_1);
    schedule_process(task_2);

    begin_execution();*/


    return;
}
