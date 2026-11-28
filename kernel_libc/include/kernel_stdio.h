#pragma once
#include "mellos/fs.h"
typedef __builtin_va_list va_list;

extern file_t* kstdin;
extern file_t* kstdout;
extern file_t* kstderr;

// tries to write to the stream first, if the stream does not exist
// it will directly write to the framebuffer
int kprintf(char *fmt, ...);
int ksprintf(char *buf, const char *fmt, ...);
int ksnprintf(char *buf, size_t size, const char *fmt, ...);
int kvsnprintf(char* buf, size_t size, const char* fmt, va_list va);
int kfputs(const char *s, file_t *stream);
int kfprintf(file_t *stream, const char *format, ...);
int kputc(int c, file_t *stream);
int kputs(const char *s);