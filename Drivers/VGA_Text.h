#ifndef VGA_TEXT_H
#define VGA_TEXT_H
void SetCursorPosRaw(uint16_t pos);
void SetCursorPos(int x, int y);
//uint16_t GetCursorPos();
//void disable_cursor();
//void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void kprint(const char* s);
void kprintChar(const char c, bool caps);
void MoveCursorLR(int i);
void MoveCursorUD(int i);
void ClearScreen(int col);
void ColLine(int line, int col);
void kprintCol(const char* s, int col);
void printError(const char* s);
void ClrLine(int line);
void ColScreen(int col);
void scrollPageUp();
#endif
