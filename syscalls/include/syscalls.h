#pragma once
#include "cpu/idt.h"

int syscall_stub (regs *r);

int sys_exit (regs *r);
int sys_fork (regs *r);
int sys_read (regs *r);
int sys_write (regs *r);
int sys_open (regs *r);
int sys_close (regs *r);
int sys_memory(regs *r);
int get_pid(regs *r);