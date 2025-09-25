#pragma once

#include "typedefs.h"

int vsnprintf(char* dest, size_t dsize, const char* fmt, va_list va);
__attribute__((format(printf, 3, 4)))
int snprintf(char* dest, size_t dsize, const char* fmt, ...);
void printf (char* s, ...);