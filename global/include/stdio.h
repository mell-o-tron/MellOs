#pragma once

// Minimal stdio declarations for kernel code; implementation is provided by mellos_libc
#include "stddef.h"

// note: use builtin va_list to avoid depending on libc headers
typedef __builtin_va_list va_list;
#define va_start(v, p) __builtin_va_start(v, p)
#define va_end(v) __builtin_va_end(v)
#define va_arg(v, t) __builtin_va_arg(v, t)
#define va_copy(d, s) __builtin_va_copy(d, s)

// Forward declare FILE to satisfy users of processes.h without dragging libc headers
struct _FILE; typedef struct _FILE FILE;
extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

int printf(const char* format, ...);
int snprintf(char* s, size_t n, const char* format, ...);
int vsnprintf(char* s, size_t n, const char* format, va_list arg);
