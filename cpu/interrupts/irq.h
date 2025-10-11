#pragma once

#include "idt.h"
#include "../../utils/typedefs.h"

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

void irq_install_handler(uint8_t irq, void (*handler)(regs *r));
void irq_uninstall_handler(uint8_t irq);
void irq_remap(void);
void irq_install();
extern  void _irq_handler(regs *r);

void irq_wait(int n);

static inline void local_irq_enable(void) {
	__asm__ volatile("sti" : : : "memory");
}

static inline void local_irq_disable(void) {
	__asm__ volatile("cli" : : : "memory");
}

/* TODO: maybe move CPU_FLAG_* and read_cpu_flags() to a different file? */

#define CPU_FLAG_INTERRUPT (1 << 9)

static inline unsigned long read_cpu_flags(void) {
    unsigned long flags;
    __asm__("pushf\n\t"
            "pop %0"
            : "=r"(flags)
            :
            : "memory");
    return flags;
}

typedef unsigned long irqflags_t;

static inline bool local_irq_enabled(irqflags_t flags) {
    return !!(flags & CPU_FLAG_INTERRUPT);
}

static inline irqflags_t local_irq_save(void) {
    irqflags_t flags = read_cpu_flags();
    local_irq_disable();
    return flags;
}

static inline void local_irq_restore(irqflags_t flags) {
    if (local_irq_enabled(flags))
        local_irq_enable();
}
