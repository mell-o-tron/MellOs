#pragma once

#ifndef __MELLOS_KERNEL_STRING_H
#define __MELLOS_KERNEL_STRING_H
#include "stdint.h"
#include "stdint.h"
#include "string.h"

void kreverse(char s[]);
size_t kstrlen(const char* s);
uint32_t kstrcmp(const char* s1, const char* s2);
char* kstrcpy(char* strDest, const char* strSrc);
_Bool kstring_starts_with(char* s, char* prefix);
char* kstr_decapitate(char* s, uint32_t n);
char* kstrdup(const char* s);
char* kstrchr(const char* s, const char f);
char* drop_after(char delimiter, char* s, _Bool include);
char* drop_after_last(char delimiter, char* s, _Bool include);
int kmemcmp(const void* s1, const void* s2, size_t n);
void* kmemchr(const void* s, int c, size_t n);
void* kmemmove(void* dest, const void* src, size_t n);
void* kmemset(void* dest, int val, size_t count);
void kmemcp(unsigned char* restrict source, unsigned char* restrict dest, size_t count);
void *kmemcpy(void * restrict to, const void * restrict from, uint32_t n);
#endif
