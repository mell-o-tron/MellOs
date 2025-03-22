#pragma once
#include "../cpu/interrupts/irq.h"

void mouse_handler(struct regs *r);
void mouse_install();