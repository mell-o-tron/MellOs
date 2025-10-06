#ifndef KERNEL_H
#define KERNEL_H

extern  void kpanic(struct regs *r);
void kpanic_message(const char* msg);

#endif