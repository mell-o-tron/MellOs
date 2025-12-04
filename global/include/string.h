#pragma once

#include "stdint.h"

// Minimal string interface for kernel code; implementations are provided by mellos_libc
void reverse(char s[]);
uint32_t strlen(const char* s);
uint32_t strcmp(const char* s1, const char* s2);
char* strcpy(char* strDest, const char* strSrc);
_Bool string_starts_with(char* s, char* prefix);
char* str_decapitate(char* s, uint32_t n);
char* strdup(const char* s);
char* drop_after(char delimiter, char* s, _Bool include);
char* drop_after_last(char delimiter, char* s, _Bool include);
int memcmp(const void* s1, const void* s2, size_t n);
void* memchr(const void* s, int c, size_t n);
void* memmove(void* dest, const void* src, size_t n);