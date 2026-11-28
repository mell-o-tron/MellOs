#pragma once
#include "cpu/idt.h"
#include "stdint.h"
void timer_phase(uint16_t hz);
void timer_handler(regs_t *r);
void timer_install();
void sleep (int ticks);
uint32_t get_ms_since_boot();
