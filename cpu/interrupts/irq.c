#include "../../utils/typedefs.h"
#include "../../drivers/port_io.h"
#ifdef VGA_VESA
#include "../../drivers/vesa/vesa_text.h"
#else
#include "../../drivers/vga_text.h"
#endif
#include "../../utils/conversions.h"
#include "idt.h"
#include "utils/bit_manip.h"

extern  void irq0();
extern  void irq1();
extern  void irq2();
extern  void irq3();
extern  void irq4();
extern  void irq5();
extern  void irq6();
extern  void irq7();
extern  void irq8();
extern  void irq9();
extern  void irq10();
extern  void irq11();
extern  void irq12();
extern  void irq13();
extern  void irq14();
extern  void irq15();

static uint16_t interrupt_mask;

void *irq_routines[16] =
{
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
};

void write_pic_mask() {
    outb(0x21, LO_8(interrupt_mask));
    outb(0xA1, HI_8(interrupt_mask));
}

void set_pic_mask(uint8_t flag) {
    interrupt_mask = SET_FLAG(interrupt_mask, flag);
    write_pic_mask();
}

void clear_pic_mask(uint8_t flag) {
    interrupt_mask = CLR_FLAG(interrupt_mask, flag);
    write_pic_mask();
}

void irq_install_handler(uint8_t irq, void (*handler)(regs *r))
{
    if (irq > 15) { return; }
    clear_pic_mask(irq);
    irq_routines[irq] = (void*)handler;
}


void irq_uninstall_handler(uint8_t irq)
{
    if (irq > 15) { return; }
    set_pic_mask(irq);
    irq_routines[irq] = 0;
}


void irq_remap(void)
{
    outb(0x20, 0x11);       // Initialize both PICs
    outb(0xA0, 0x11);       
    outb(0x21, 0x20);       // Set vector offsets
    outb(0xA1, 0x28);
    outb(0x21, 0x04);       // tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outb(0xA1, 0x02);       // tell Slave PIC its cascade identity (0000 0010)
    outb(0x21, 0x01);       // have the PICs use 8086 mode (and not 8080 mode)
    outb(0xA1, 0x01);
    
    // Mask all interrupts except IRQ2 (interrupts coming from the slave PIC)
    interrupt_mask = CLR_FLAG(0xFFFF, 2);
    write_pic_mask();
}

// Originally this was an array with size 15, but I have strong
//reasons to believe it should be 16. If this causes issues, revert this change.
// - Perk
static volatile int currentInterrupts[16];

void irq_install()
{
    irq_remap();

    idt_set_gate(32, (unsigned)irq0, 0x08, 0x8E);
    idt_set_gate(33, (unsigned)irq1, 0x08, 0x8E);
    idt_set_gate(34, (unsigned)irq2, 0x08, 0x8E);
    idt_set_gate(35, (unsigned)irq3, 0x08, 0x8E);
    idt_set_gate(36, (unsigned)irq4, 0x08, 0x8E);
    idt_set_gate(37, (unsigned)irq5, 0x08, 0x8E);
    idt_set_gate(38, (unsigned)irq6, 0x08, 0x8E);
    idt_set_gate(39, (unsigned)irq7, 0x08, 0x8E);
    idt_set_gate(40, (unsigned)irq8, 0x08, 0x8E);
    idt_set_gate(41, (unsigned)irq9, 0x08, 0x8E);
    idt_set_gate(42, (unsigned)irq10, 0x08, 0x8E);
    idt_set_gate(43, (unsigned)irq11, 0x08, 0x8E);
    idt_set_gate(44, (unsigned)irq12, 0x08, 0x8E);
    idt_set_gate(45, (unsigned)irq13, 0x08, 0x8E);
    idt_set_gate(46, (unsigned)irq14, 0x08, 0x8E);
    idt_set_gate(47, (unsigned)irq15, 0x08, 0x8E);

    for(int i = 0; i < 16; i++){
        currentInterrupts[i] = 0;
    }
}


extern  void _irq_handler(regs *r)
{
    currentInterrupts[r -> int_no - 32] = 1;
    void (*handler)(struct regs *r);


    handler = (void (*)(regs*))irq_routines[r->int_no - 32];
    if (handler)
    {
        handler(r);
    }


    if (r->int_no >= 40)
    {
        outb(0xA0, 0x20);   // END OF INTERRUPT command to PIC2
    }


    outb(0x20, 0x20);       // END OF INTERRUPT command to PIC1
}

void irq_wait(int n){
    while(!currentInterrupts[n]){};
    currentInterrupts[n] = 0;
}