#include "autoconf.h"
// INTERRUPT SERVICE ROUTINES

//(Shamelessely stolen | Adapted) from http://www.osdever.net/

#include "cpu/idt.h"
#ifdef CONFIG_GFX_VESA
#else
#include "vga_text.h"
#endif
#include "cpu/isr.h"
#include "mellos/kernel/kernel.h"
#include "syscalls.h"

#include <port_io.h>

bool probing;
uint8_t probe_type;
bool probe_result;

char* exception_messages[32] = {"Division By Zero",
                                "Debug",
                                "Non Maskable Interrupt",
                                "Breakpoint",
                                "Into Detected Overflow",
                                "Out of Bounds",
                                "Invalid Opcode",
                                "No Coprocessor",
                                "Double Fault",
                                "Coprocessor Segment Overrun",
                                "Bad TSS",
                                "Segment Not Present",
                                "Stack Fault",
                                "General Protection Fault",
                                "Page Fault",
                                "Unknown Interrupt",
                                "Coprocessor Fault",
                                "Alignment Check",
                                "Machine Check",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved"};

__attribute__((section(".low.text"))) void isrs_install() {
	idt_set_gate(0, (unsigned)_isr0, 0x08, 0x8E);
	idt_set_gate(1, (unsigned)_isr1, 0x08, 0x8E);
	idt_set_gate(2, (unsigned)_isr2, 0x08, 0x8E);
	idt_set_gate(3, (unsigned)_isr3, 0x08, 0x8E);
	idt_set_gate(4, (unsigned)_isr4, 0x08, 0x8E);
	idt_set_gate(5, (unsigned)_isr5, 0x08, 0x8E);
	idt_set_gate(6, (unsigned)_isr6, 0x08, 0x8E);
	idt_set_gate(7, (unsigned)_isr7, 0x08, 0x8E);
	idt_set_gate(8, (unsigned)_isr8, 0x08, 0x8E);
	idt_set_gate(9, (unsigned)_isr9, 0x08, 0x8E);
	idt_set_gate(10, (unsigned)_isr10, 0x08, 0x8E);
	idt_set_gate(11, (unsigned)_isr11, 0x08, 0x8E);
	idt_set_gate(12, (unsigned)_isr12, 0x08, 0x8E);
	idt_set_gate(13, (unsigned)_isr13, 0x08, 0x8E);
	idt_set_gate(14, (unsigned)_isr14, 0x08, 0x8E);
	idt_set_gate(15, (unsigned)_isr15, 0x08, 0x8E);
	idt_set_gate(16, (unsigned)_isr16, 0x08, 0x8E);
	idt_set_gate(17, (unsigned)_isr17, 0x08, 0x8E);
	idt_set_gate(18, (unsigned)_isr18, 0x08, 0x8E);
	idt_set_gate(19, (unsigned)_isr19, 0x08, 0x8E);
	idt_set_gate(20, (unsigned)_isr20, 0x08, 0x8E);
	idt_set_gate(21, (unsigned)_isr21, 0x08, 0x8E);
	idt_set_gate(22, (unsigned)_isr22, 0x08, 0x8E);
	idt_set_gate(23, (unsigned)_isr23, 0x08, 0x8E);
	idt_set_gate(24, (unsigned)_isr24, 0x08, 0x8E);
	idt_set_gate(25, (unsigned)_isr25, 0x08, 0x8E);
	idt_set_gate(26, (unsigned)_isr26, 0x08, 0x8E);
	idt_set_gate(27, (unsigned)_isr27, 0x08, 0x8E);
	idt_set_gate(28, (unsigned)_isr28, 0x08, 0x8E);
	idt_set_gate(29, (unsigned)_isr29, 0x08, 0x8E);
	idt_set_gate(30, (unsigned)_isr30, 0x08, 0x8E);
	idt_set_gate(31, (unsigned)_isr31, 0x08, 0x8E);

	idt_set_gate(0x80, (unsigned)_syscall, 0x08, 0x8E);
}

extern void _fault_handler(struct regs* r) {
	if (r->int_no == 0x80) {
		syscall_stub(r);
		outb(0x20, 0x20);
		return;
	}

	if (r->int_no < 32) {
		if (probing) {
			if (probe_type == PROBE_TYPE_MMX) {
				probe_result = true;
			} else {
				probe_result = false;
			}

			probing = false;
		}
		// kprint("Received interrupt: ");
		// kprint_dec(r->int_no);
		// unsigned int cr2;
		// asm volatile("mov %%cr2, %0" : "=r" (cr2));
		// kprint("\nCR2 Register: ");
		// kprint_hex(cr2);
		// unsigned int stack_value;
		// asm volatile("movl 4(%%ebp), %0" : "=r" (stack_value));
		// kprint("\nValue on stack: ");
		// kprint_hex(stack_value);
		// unsigned int return_pointer;
		// asm volatile("movl 8(%%ebp), %0" : "=r" (return_pointer));
		// kprint("\nReturn Pointer: ");
		// kprint_hex(return_pointer);
		// while(1);
		kpanic(r);
	}
}
