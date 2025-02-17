#include "../../utils/typedefs.h"
#include "../../drivers/port_io.h"
#include "../../drivers/vga_text.h"
#include "../../utils/conversions.h"
#include "../interrupts/irq.h"
#include "../interrupts/idt.h"
#include "../../processes/processes.h"
#include "../../drivers/keyboard.h"

int timer_ticks = 0;
int seconds = 0;

extern bool keyboard_enabled;

void timer_phase(int hz)
{
	int divisor = 1193180 / hz;	   /* Calculate our divisor */
	outb(0x43, 0x36);			 /* Set our command byte 0x36 */
	outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
	outb(0x40, divisor >> 8);	 /* Set high byte of divisor */
}

void timer_handler(regs *r)
{
	/* Increment our 'tick count' */
	timer_ticks++;

	if (timer_ticks % 18 == 0)
	{
		seconds++;
		// kprint(tostring_inplace(seconds, 10));
	}

	if (timer_ticks % 36 == 0) {
		scheduler_daemon();
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

}


void sleep (int ticks){
    int startTicks = timer_ticks;
    while(timer_ticks < startTicks + ticks){}
    return;
}


void timer_install()
{
	irq_install_handler(0, timer_handler);
}
