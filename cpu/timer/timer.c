#include "utils/typedefs.h"
#include "drivers/port_io.h"
#ifdef VGA_VESA
#include "drivers/vesa/vesa_text.h"
#else
#include "drivers/vga_text.h"
#endif
#include "utils/conversions.h"
#include "cpu/interrupts/irq.h"
#include "cpu/interrupts/idt.h"
#include "processes/processes.h"
#include "drivers/keyboard.h"
#include "utils/format.h"

volatile uint32_t timer_ticks = 0;
int seconds = 0;
volatile uint16_t current_hz = 0;

extern bool keyboard_enabled;

void timer_phase(uint16_t hz)
{
	irqflags_t irqf = local_irq_save();
	current_hz = hz;
	uint32_t div32 = 1193180u / (hz ? hz : 1u);
    uint16_t divisor = (uint16_t)div32;

    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
	local_irq_restore(irqf);
}

void timer_handler(regs *r)
{
	/* Increment our 'tick count' */
	timer_ticks++;

	if (timer_ticks % current_hz == 0)
	{
		seconds++;
        // printf("%d seconds have passed\n", seconds);
	}

	if (keyboard_enabled) {
		// if the keyboard is enabled, print the top of the keyboard buffer.
		char c = get_from_kb_buffer();
		switch (c) {
			case 0: break; // buffer is empty
			case 8: move_cursor_LR(-1); kprint_char(' ', 0);move_cursor_LR(-1); break;	// backspace
			default: kprint_char(c, 0);
		}

		char a = get_from_act_buffer();
		switch (a) {
			case 'U': move_cursor_UD(-1);break;
			case 'D': move_cursor_UD(+1);break;
			case 'L': move_cursor_LR(-1);break;
			case 'R': move_cursor_LR(+1);break;
		}
	}
	else {
		// if the keyboard is not enabled, older keystrokes are eliminated at a rate of 2 chars a second.
		if (timer_ticks % 9 == 0){
			rem_from_kb_buffer();
			rem_from_act_buffer();
		}
	}

    if (timer_ticks % 1 == 0) {
		scheduler_daemon();
	}
}


void sleep (int ticks){
    int startTicks = timer_ticks;
    while(timer_ticks < startTicks + ticks){
		;
	}
    return;
}


void timer_install()
{
	irq_install_handler(0, timer_handler);
}
