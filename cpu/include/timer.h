#pragma once
#include "cpu/idt.h"
void timer_phase(int hz);
void timer_handler(regs *r);
void timer_install();
void sleep (int ticks);