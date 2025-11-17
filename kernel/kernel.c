/**********************
 * TEXT MODE: 0xB8000 *
 * GR.  MODE: 0xA000  *
 *********************/

#include "mellos/kernel/memory_mapper.h"
#include "mellos/kernel/multiboot_tags.h"
#include "memory_area_spec.h"
#include "stddef.h"
// #include "../utils/error_handling.h"                // read docs!
#include "colours.h"
#include "conversions.h"

#include "stdio.h"

#include "allocator.h"
#include "autoconf.h"
#include "cpu/gdt.h"
#include "cpu/idt.h"
#include "cpu/irq.h"
#include "cpu/isr.h"
#include "dynamic_mem.h"
#include "init.h"
#include "mellos/kernel/kernel_stdio.h"
#include "paging/paging.h"
#include "paging/pat.h"
#include "port_io.h"
#include "timer.h"

#include "keyboard.h"
#ifdef CONFIG_GFX_VESA
#include "mouse.h"
#include "vesa.h"
#include "vesa_text.h"
#else
#include "init.h"
#include "vga_text.h"
#endif

// Provide a weak default for tests when not linked
int __attribute__((weak)) run_all_mem_tests(void) {
	return 0;
}

// TEXT-mode clear-screen shim to match VESA kclear_screen signature
#ifndef CONFIG_GFX_VESA
static void vga_kclear_screen(void) {
	clear_screen_col(DEFAULT_COLOUR);
}
#endif
// #include "../utils/assert.h"

// PROCESSES
#include "../processes/processes.h"
// #include "../processes/stack_utils.h"

// MATH
// #include "../utils/math.h"

// DISK
// #include "../drivers/disk.h"

// SHELL
#include "../shell/include/shell/shell.h"

// FILE SYSTEM

// TEXT_EDITOR

// TESTS
#include "../test/fs_test.h"
#include "uart.h"

#ifdef CONFIG_GFX_VESA
__attribute__((section(".multiboot")))
#define MULTIBOOT_HEADER_MAGIC 0x1BADB002

#define CHECK_FLAG(flags, bit) ((flags) & (1 << (bit)))

#define MULTIBOOT_HEADER_FLAGS 0x00000007
const uint32_t multiboot_header[] = {
    MULTIBOOT_HEADER_MAGIC, MULTIBOOT_HEADER_FLAGS,
    -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS) & 0xFFFFFFFF,
    // aout kludge (unused)
    0, 0, 0, 0, 0,
    // video mode
    0,               // Linear graphics please?
    CONFIG_GFX_HRES, // Preferred width
    CONFIG_GFX_VRES, // Preferred height
    CONFIG_GFX_BPP   // Preferred pixel depth
};
#else
__attribute__((section(".multiboot")))
#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
#define MULTIBOOT_HEADER_FLAGS 0x0
const uint32_t multiboot_header[] = {MULTIBOOT_HEADER_MAGIC, MULTIBOOT_HEADER_FLAGS,
                                     -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS) &
                                         0xFFFFFFFF};
#endif

extern const char KPArt[];
extern const char Fool[];

bool keyboard_enabled = false; // maybe put this in some "state" struct?

void khang() {
	for (;;)
		;
}

#pragma GCC push_options
#pragma GCC optimize("O0")

// This function has to be self contained - no dependencies to the rest of the kernel!
_Noreturn void _kpanic(const char* msg, unsigned int int_no, regs* r);
_Noreturn void kpanic_message(const char* msg) {
	_kpanic(msg, 0, NULL);
}

extern void kpanic(struct regs* r) {
	_kpanic(exception_messages[r->int_no], r->int_no, r);
}
__attribute__((section(".low.text"))) _Noreturn void _kpanic(const char* msg, unsigned int int_no,
                                                             regs* r) {
	const char* components[] = {
	    KPArt,
	    "Kernel panic: ",
	    msg,
	};
	char buf[256];
#ifdef CONFIG_GFX_VESA
	snprintf(buf, 255, "%s %s %s%i%s", components[1], components[2], "(", int_no, ")");

	fb_clear_screen_col_VESA(VESA_RED, vga_fb);

	if (CONFIG_GFX_HRES > 1200) {
		fb_draw_string(16, 16, buf, VESA_DARK_GREY, 3, 3, vga_fb);
	} else {
		fb_draw_string(16, 16, buf, VESA_DARK_GREY, 2, 2, vga_fb);
	}
#else
#define ERRCOL 0x47 // Lightgrey on Lightred
#define VGAMEM (unsigned char*)0xB8000;

	char panicscreen[4000];

	int psidx = 0; // Index to access panicscreen
	int idx = 0;
	snprintf(buf, 255,
	         "\n\nEAX=%08x  EBX=%08x  ECX=%08x  EDX=%08x\n"
	         "ESI=%08x  EDI=%08x  EBP=%08x  ESP=%08x\n"
	         "EIP=%08x  EFLAGS=%08x CR2=%08x\n"
	         "CS=%04x  DS=%04x  ES=%04x  FS=%04x  GS=%04x  SS=%04x\n"
	         "INT=%02x ERR=%08x",
	         r->eax, r->ebx, r->ecx, r->edx, r->esi, r->edi, r->ebp, r->esp, r->eip, r->eflags,
	         r->cr2, r->cs, r->ds, r->es, r->fs, r->gs, r->ss, int_no, r->err_code);

	for (int x = 0; x < sizeof(components) / sizeof(char*); x++) {
		idx = 0;
		while (components[x][idx] != 0) {
			if (components[x][idx] == '\n') {
				do {
					panicscreen[psidx] = ' ';
					psidx++;
				} while ((psidx + 1) % 80 != 0);
			} else
				panicscreen[psidx] = components[x][idx];
			psidx++;
			idx++;
		}
	}

	idx = 0;
	while (buf[idx] != 0) {
		if (buf[idx] == '\n') {
			do {
				panicscreen[psidx] = ' ';
				psidx++;
			} while ((psidx + 1) % 80 != 0);
		} else {
			panicscreen[psidx] = buf[idx];
		}
		psidx++;
		idx++;
	}

	unsigned char* write = VGAMEM;

	for (int i = 0; i < 4000; i++) {
		*write++ = panicscreen[i];
		*write++ = ERRCOL;
	}
#endif

	// Disables the flashing cursor because that's annoying imo
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);

	for (;;)
		;
}

#pragma GCC pop_options

PD_FLAGS page_directory_flags = PD_PRESENT | PD_READWRITE;
PT_FLAGS first_page_table_flags = PT_PRESENT | PT_READWRITE;

extern int top_of_stack();

void test_task() {
	kprint_col("test task print\n", DEFAULT_COLOUR);

	return;
}

void task_1() {
	int i = 0;
	for (;;) {
		printf("Hello there! %d\n", i);
		i++;
		sleep(1);
	}
}

void task_2() {
	int i = 0;
	for (;;) {
		printf("BOIA DE %d\n", i);
		i++;
		sleep(1);
	}
}

__attribute__((section(".low.bss")))
MultibootTags mb_tags;

// char test[0xe749] = {1};
allocator_t allocator;
__attribute__((section(".low.bss"))) uint32_t fb_addr;
__attribute__((section(".entry"))) extern void main(uint32_t multiboot_tags_addr) {
	// Early guard to prevent NULL deref from bad boot handoff
	if (multiboot_tags_addr == 0) {
		kprintf("FATAL: multiboot_tags_addr is NULL at entry\n");
		kpanic_message("Null Multiboot pointer");
	}

	mb_tags = *((MultibootTags*)multiboot_tags_addr);
	fb_addr = (uint32_t)get_multiboot_framebuffer_addr((MultibootTags*)multiboot_tags_addr);
#ifdef CONFIG_GFX_VESA
	init_memory_mapper((MultibootTags*)multiboot_tags_addr, (PIXEL*)fb_addr, CONFIG_GFX_BPP);
#else
	init_memory_mapper((MultibootTags*)multiboot_tags_addr, 0, 0);
#endif

#ifdef CONFIG_GFX_VESA
	init_paging(fb_addr, (MultibootTags*)multiboot_tags_addr);
#else
	init_paging(0, (MultibootTags*)multiboot_tags_addr);
#endif
}

__attribute__((section(".text"))) _Noreturn void higher_half_main(uintptr_t multiboot_tags_addr) {
#ifdef CONFIG_GFX_VESA
	init_assertions(&clear_screen_col, &set_cursor_pos_raw, &kclear_screen);
#else
	init_assertions(&clear_screen_col, &set_cursor_pos_raw, &vga_kclear_screen);
#endif

	MultibootTags* multiboot_tags = (MultibootTags*)multiboot_tags_addr;
#ifdef CONFIG_GFX_VESA
	fb_addr = get_multiboot_framebuffer_addr((MultibootTags*)multiboot_tags_addr);

	if (mb_tags.flags & (1 << 12)) {
		Hres = mb_tags.framebuffer_width;
		Vres = mb_tags.framebuffer_height;
		Pitch = mb_tags.framebuffer_pitch; // Convert to pixels
	} else {
		Hres = CONFIG_GFX_HRES;
		Vres = CONFIG_GFX_VRES;
		Pitch = sizeof(PIXEL) * CONFIG_GFX_HRES;
	}
#endif
	kprintf("lower: %X\n", mb_tags.mem_lower);
	kprintf("upper: %X\n", mb_tags.mem_upper);

	kprintf("map:\n");

	// Truncate to 32-bit physical address space explicitly (we run in 32-bit mode)

	// gdt_init();
	// idt_install();
	// irq_install();

#ifdef CONFIG_GFX_PAT // PAT became standard in i686 (Pentium Pro/2/3)
	setup_pat();      // Sets up the Page Attribute Table
#endif
	// Maps a few pages for future use. Until we have a page manager, we just have a fixed number of
	// pages for(int32_t i = 0; i < NUM_MANY_DIRECTORIES; i++){
	//     uintptr_t va = MAPPED_KERNEL_START + (2U + i) * 0x400000;
	//     put_page_table_to_directory(, lots_of_pages[i], i + 2, va, first_page_table_flags,
	//     page_directory_flags);
	// }

	asm volatile("cli");
	gdt_init();
	idt_install();
	isrs_install();
	irq_install();

	timer_phase(60);
	timer_install();
	set_cursor_pos_raw(0);

	// Initialize dynamic memory allocation
	init_allocators();
	map_memory();
#ifdef CONFIG_GFX_VESA
	_vesa_framebuffer_init(FRAMEBUFFER_VIRT_START);
	_vesa_text_init();
	mouse_install();
#endif
	kb_install();
#ifdef CONFIG_CONFIG_SERIAL
	uart_init();
#endif
	asm volatile("sti");
#ifdef MELLOS_ENABLE_TESTS
	kprint("MellOS Debug mode:\n\n");

	kprint("Running fs tests... ");
	int failed_fs_tests = run_all_fs_tests();
	kprint(tostring_inplace(failed_fs_tests, 10));
	kprint(" failed\n");

	kprint("Running mem tests... ");
	int failed_mem_tests = run_all_mem_tests();
	kprint("0x");
	kprint(tostring_inplace(failed_mem_tests, 16));
	kprint(" failed\n");

	kprint("\n\n ENTERING COMMAND MODE...\n");
	if (failed_fs_tests != 0 || failed_mem_tests != 0) {
		kprint_col("TESTS FAILED!!", DEFAULT_COLOUR);

		for (;;) {
			;
		}
	} else {
		kprint_col("All tests passed!", DEFAULT_COLOUR);
	}
#endif
	asm volatile("cli");
	kprintf("\n\n ENTERING COMMAND MODE...\n");

	void* code_loc2 = kmalloc(10);

	asm volatile("sti");
	if (code_loc2 == NULL) {
		kprint_col("SLAB ALLOC TEST FAILED!!", DEFAULT_COLOUR);
	} else {
		kfree(code_loc2);
	}
	// sleep(100);
#ifdef CONFIG_GFX_VESA
	kclear_screen();
#else
	clear_screen_col(DEFAULT_COLOUR);
#endif

	set_cursor_pos_raw(0);

	kprintf(Fool);

	// Initialize the process scheduler and set this as the first process
	init_scheduler();

	load_shell();
	// init_text_editor("test_file");
	while (1) {
	}
}
