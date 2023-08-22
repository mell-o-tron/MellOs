#ifndef VGA_TEXT_H
#define VGA_TEXT_H
#include "../Utils/Typedefs.h"
void SetCursorPosRaw(uint16_t pos);
void SetCursorPos(int x, int y);
//uint16_t GetCursorPos();
//void disable_cursor();
//void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);

int  display_tty(char* tty);
int  display_tty_line(char* tty, int line);

int  clear_tty(int col, char *tty);

int kprint(const char* s);
int  kprintChar(const char c, bool caps);
void MoveCursorLR(int i);
void MoveCursorUD(int i);

int  col_tty_line(int line, int col, char* tty);
void kprintCol(const char* s, int col);
void printError(const char* s);
int  clr_tty_line(int line, char *tty);
void ColScreen(int col);
void scrollPageUp();
#endif
