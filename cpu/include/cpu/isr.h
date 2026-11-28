#pragma once
#include "stdint.h"
#include "stdbool.h"
// Shamelessely stolen, with some degree of adaptation, from http://www.osdever.net/

extern void _fault_handler(struct regs* r);
void isrs_install();
extern void _isr0();
extern void _isr1();
extern void _isr2();
extern void _isr3();
extern void _isr4();
extern void _isr5();
extern void _isr6();
extern void _isr7();
extern void _isr8();
extern void _isr9();
extern void _isr10();
extern void _isr11();
extern void _isr12();
extern void _isr13();
extern void _isr14();
extern void _isr15();
extern void _isr16();
extern void _isr17();
extern void _isr18();
extern void _isr19();
extern void _isr20();
extern void _isr21();
extern void _isr22();
extern void _isr23();
extern void _isr24();
extern void _isr25();
extern void _isr26();
extern void _isr27();
extern void _isr28();
extern void _isr29();
extern void _isr30();
extern void _isr31();
extern void _syscall();

#define PROBE_TYPE_MMX 1

// todo: probe_features
extern bool probing;
extern uint8_t probe_type;
extern bool probe_result;

extern char* exception_messages[32];
