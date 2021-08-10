#ifndef TIMER_H
#define TIMER_H
void timer_phase(int hz);
void timer_handler(struct regs *r);
void timer_install();
void sleep (int ticks);
#endif
