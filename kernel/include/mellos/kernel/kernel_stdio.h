#pragma once
#include "streams.h"
#include "stdint.h"
typedef __builtin_va_list va_list;
// tries to write to the stream first, if the stream does not exist
// it will directly write to the framebuffer
int kprintf(char *fmt, ...);
int ksprintf(char *buf, const char *fmt, ...);
int ksnprintf(char *buf, size_t size, const char *fmt, ...);
int kvsnprintf(char* buf, size_t size, const char* fmt, va_list va);
int kfputs(const char *s, FILE *stream);
int kfprintf(FILE *stream, const char *format, ...);
int kputc(int c, FILE *stream);
int kputs(const char *s);