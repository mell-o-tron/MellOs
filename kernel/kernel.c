/*********************
* TEXT MODE: 0xB8000 *
* GR.  MODE: 0xA000  *
*********************/


#include "kernel.h"
#include "memory_area_spec.h"
#include "../utils/typedefs.h"
#include "multiboot_tags.h"
#include "memory_mapper.h"
// #include "../utils/error_handling.h"                // read docs!
#include "../misc/colours.h"
#include "../utils/conversions.h"
// #include "../utils/string.h"
#include "paging_utils.h"

#include "format.h"

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

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

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
void _kpanic(const char* msg, unsigned int int_no);
void kpanic_message(const char* msg) {
    _kpanic(msg, 0);
}

extern  void kpanic(struct regs *r) {
    _kpanic(exception_messages[r->int_no], r->int_no);
}

void _kpanic(const char* msg, unsigned int int_no) {
    const char* components[] = {
        KPArt,
        "Kernel panic: ",
        msg,
    };
#if VGA_VESA
    char buf[256];
    snprintf(buf, 255, "%s %s %s%i%s", components[1], components[2], "(", int_no, ")");

    fb_clear_screen_col_VESA(VESA_RED, vga_fb);
    fb_draw_string(16, 16, buf, VESA_DARK_GREY, 3, 3, vga_fb);
#else
    #define ERRCOL 0x47 // Lightgrey on Lightred
    #define VGAMEM (unsigned char*)0xB8000;

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
#endif



    // Disables the flashing cursor because that's annoying imo
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);

    for(;;);
}

#pragma GCC pop_options

#define PAGE_LENGTH 4096
uint32_t page_directory[1024]       __attribute__((aligned(4096)));
uint32_t first_page_table[1024]     __attribute__((aligned(4096)));
uint32_t second_page_table[1024]    __attribute__((aligned(4096)));
#define NUM_MANY_DIRECTORIES (uint32_t)512
uint32_t lots_of_pages[NUM_MANY_DIRECTORIES][1024]  __attribute__((aligned(4096)));
#ifdef VGA_VESA
#define NUM_FB_DIRECTORIES (uint32_t)2 // FB is 8100 KB (1920x1080x4), so 2 pages are enough
unsigned int framebuffer_pages[NUM_FB_DIRECTORIES][1024]     __attribute__((aligned(4096)));
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
    int i = 0;
    for (;;){
        printf("Hello there! %d\n", i);
        i++;
        sleep(1);
    }
}

void task_2(){
    int i = 0;
    for (;;){
        printf("BOIA DE %d\n", i);
        i++;
        sleep(1);
    }
}

// char test[0xe749] = {1};
allocator_t allocator;

asmlinkage void main(uint32_t multiboot_tags_addr){
#ifdef VGA_VESA

    uintptr_t fb_addr = get_multiboot_framebuffer_addr((MultibootTags*)multiboot_tags_addr);
    const uint32_t framebuffer_addr = 0x400000 * (2 + NUM_MANY_DIRECTORIES); // Addr of the next page that will be added
#endif
    MultibootTags* multiboot_tags = (MultibootTags*)multiboot_tags_addr;
    printf("lower: %i\n", multiboot_tags->mem_upper);
    printf("upper: %i\n", multiboot_tags->mem_lower);

    printf("map:\n");




    /*
        +-------------------+
-4      | size              |
        +-------------------+
0       | base_addr         |
8       | length            |
16      | type              |
        +-------------------+
    */
#ifdef VGA_VESA
    init_memory_mapper(multiboot_tags, (void *)fb_addr, BPP);
#elif VGA_TEXT
    init_memory_mapper(multiboot_tags, NULL, BPP);
#endif
    MemoryArea memory_area = get_largest_free_block();


    // Truncate to 32-bit physical address space explicitly (we run in 32-bit mode)

    // identity-maps 0x0 to 8MB (i.e. 0x800000 - 1)
    init_paging(page_directory, first_page_table, second_page_table, memory_area.start);

    // Sets up the Page Attribute Table
    setup_pat();

    // Maps a few pages for future use. Until we have a page manager, we just have a fixed number of pages
    for(int32_t i = 0; i < NUM_MANY_DIRECTORIES; i++){
        uintptr_t va = MAPPED_KERNEL_START + (2U + i) * 0x400000;
        add_page_directory(page_directory, lots_of_pages[i], i + 2, va, first_page_table_flags, page_directory_flags);
    }


    #ifdef VGA_VESA
    // Map two pages for the framebuffer

    for (uint32_t i = 0; i < NUM_FB_DIRECTORIES; i++){
        uintptr_t phys_base = (fb_addr & ~0x3FFFFF) + (i * 0x400000);
        add_page_directory(page_directory, framebuffer_pages[i],
            2 + NUM_MANY_DIRECTORIES + i,
            phys_base,
            framebuffer_page_tflags, framebuffer_page_dflags);
    }
    #endif

    gdt_init();
    idt_install();
    isrs_install();
    irq_install();
    // asm volatile("hlt");


    asm volatile ("sti");
    timer_phase(60);
    timer_install();
    set_cursor_pos_raw(0);

    //allocator.granularity = 512;
    //assign_kmallocator(&allocator);
    set_dynamic_mem_loc ((void*)memory_area.start);
    if (!buddy_init(memory_area.start, memory_area.length)) {
        printf("Buddy allocator fault.\n");
        // todo: error handling
        //asm volatile ("hlt");
    }
    //set_kmalloc_bitmap((bitmap_t) 0x800000, 100000000);   // dynamic memory allocation setup test. Starting position is at 0x800000 as we avoid interfering with the kernel at 0x400000
    #ifdef VGA_VESA
    // set_dynamic_mem_loc ((void*)framebuffer_end);

    //MultibootTags* multiboot_tags = (MultibootTags*)multiboot_tags_addr;
    Hres = multiboot_tags->framebuffer_width;
    Vres = multiboot_tags->framebuffer_height;
    Pitch = multiboot_tags->framebuffer_pitch / BYTES_PER_PIXEL; // Convert to pixels
    _vesa_framebuffer_init(fb_addr);
    _vesa_text_init();
    mouse_install();
    #endif


    kb_install();

    // kprint("Running fs tests... ");
    // int failed_fs_tests = run_all_fs_tests();
    // kprint(tostring_inplace(failed_fs_tests, 10));
    // kprint(" failed\n");

    // printf("Running mem tests... ");
    // int failed_mem_tests = run_all_mem_tests();
    // printf("%016x", failed_mem_tests);
    // printf(" failed\n");

    // printf("\n\n ENTERING COMMAND MODE...\n");


    // sleep(30);

    void *code_loc2 = kmalloc(10);
    if (code_loc2 == NULL){
        kprint_col("SLAB ALLOC TEST FAILED!!", DEFAULT_COLOUR);

        for (;;){;}
    } else {
        kfree(code_loc2, 10);
    }

    #ifdef VGA_VESA
    kclear_screen();
    #else
    clear_screen_col(DEFAULT_COLOUR);
    #endif

    set_cursor_pos_raw(0);

    kprint(Fool);

    // Initialize the process scheduler and set this as the first process
    init_scheduler();

    load_shell();
    // init_text_editor("test_file");


    return;
}
