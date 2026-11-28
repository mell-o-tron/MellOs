#pragma once
#include "cpu/idt.h"

int syscall_stub(regs_t* r);

int sys_exit(regs_t* r);
int sys_fork(regs_t* r);
int sys_read(regs_t* r);
int sys_write(regs_t* r);
int sys_open(regs_t* r);
int sys_close(regs_t* r);
int sys_mmap(regs_t* r);
int sys_munmap(regs_t* r);
int sys_mprotect(regs_t* r);
int get_pid(regs_t* r);