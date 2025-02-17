#pragma once

void keyboard_handler(struct regs *r);
void kb_install();

void add_to_kb_buffer(char c, bool is_uppercase);
char get_from_kb_buffer();
void rem_from_kb_buffer();

void add_to_act_buffer(char action);
char get_from_act_buffer();
void rem_from_act_buffer();
