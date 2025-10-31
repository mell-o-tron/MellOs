#pragma once
#include "stdio.h"

int kprintf(char *fmt, ...);
int ksprintf(char *buf, const char *fmt, ...);
int ksnprintf(char *buf, size_t size, const char *fmt, va_list va);
int kvsnprintf(char* buf, size_t size, const char* fmt, va_list va);