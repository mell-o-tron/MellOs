#ifndef KEYBOARD_H
#define KEYBOARD_H
void keyboard_handler(struct regs *r);
void kb_install();
void get_keyboard(struct regs *r);
void await(unsigned char trigger);
void keyboard_proc();
void keyboard_old();
#endif
