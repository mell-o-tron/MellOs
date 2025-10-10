#pragma once

#include "stdint.h"

void reverse(char s[]);
uint32_t strlen(const char* s);
uint32_t strcmp(const char* s1, const char* s2);
char * strcpy(char *strDest, const char *strSrc);
_Bool string_starts_with(char *s, char *prefix);

char* str_decapitate(char *s, uint32_t n);
