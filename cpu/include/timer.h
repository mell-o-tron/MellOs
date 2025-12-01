#pragma once
#include "cpu/idt.h"
#include "stdint.h"
void timer_phase(int hz);
void timer_handler(regs *r);
void timer_install();
void sleep (int ticks);
uint32_t get_ms_since_boot();
