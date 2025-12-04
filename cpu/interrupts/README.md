[← CPU README](../README.md) | [← Main README](../../README.md)

# interrupts

This directory contains code for interrupt handling in MellOs, including IRQ management and interrupt service routines (ISRs).

## irq.c / irq.h
- **irq0() ... irq15()**: Handlers for hardware IRQs 0-15.
- **irq_install_handler(int irq, void (\*handler)(regs \*r))**: Installs a custom handler for a given IRQ.
- **irq_uninstall_handler(int irq)**: Removes a custom handler for a given IRQ.
- **irq_install()**: Installs the IRQ handlers.
- **_irq_handler(regs \*r)**: Main IRQ handler called from assembly stubs.
- **irq_wait(int n)**: Waits for a given IRQ to occur.
- **local_irq_enable(), local_irq_disable(), local_irq_restore(irqflags_t flags)**: Inline functions/macros to enable/disable/restore CPU interrupt flags.
- **irqflags_t**: Type for storing interrupt flag state.
