#include "autoconf.h"
#include "port_io.h"
#include "stdbool.h"
#include "stdint.h"
#ifdef CONFIG_GFX_VESA
#include "vesa_text.h"
#else
#include "vga_text.h"
#endif
#include "conversions.h"
#include "cpu/idt.h"
#include "cpu/irq.h"
#include "keyboard.h"
#include "processes.h"

volatile int timer_ticks = 0;
int seconds = 0;

extern bool keyboard_enabled;

void timer_phase(int hz) {
	int divisor = 1193182 / hz; /* Calculate our divisor */
	outb(0x43, 0x36);           /* Set our command byte 0x36 */
	outb(0x40, divisor & 0xFF); /* Set low byte of divisor */
	outb(0x40, divisor >> 8);   /* Set high byte of divisor */
}

void timer_handler(regs* r) {
	/* Increment our 'tick count' */
	timer_ticks++;

	if (timer_ticks % 18 == 0) {
		seconds++;
		// kprint(tostring_inplace(seconds, 10));
	}

	if (keyboard_enabled) {
		// if the keyboard is enabled, print the top of the keyboard buffer.
		char c = get_from_kb_buffer();
		switch (c) {
		case 0:
			break; // buffer is empty
		case 8:
			move_cursor_LR(-1);
			kprint_char(' ', 0);
			move_cursor_LR(-1);
			break; // backspace
		default:
			kprint_char(c, 0);
		}

		char a = get_from_act_buffer();
		switch (a) {
		case 'U':
			move_cursor_UD(-1);
			break;
		case 'D':
			move_cursor_UD(+1);
			break;
		case 'L':
			move_cursor_LR(-1);
			break;
		case 'R':
			move_cursor_LR(+1);
			break;
		}
	} else {
		// if the keyboard is not enabled, older keystrokes are eliminated at a rate of 2 chars a
		// second.
		if (timer_ticks % 9 == 0) {
			rem_from_kb_buffer();
			rem_from_act_buffer();
		}
	}

	if (timer_ticks % 1 == 0) {
		scheduler_daemon();
	}
}

void sleep(int ticks) {
	int startTicks = timer_ticks;
	while (timer_ticks < startTicks + ticks) {
		;
	}
	return;
}

void timer_install() {
	irq_install_handler(0, timer_handler);
}

uint32_t get_ms_since_boot() {
	return (timer_ticks * 1000) / 60;
}
