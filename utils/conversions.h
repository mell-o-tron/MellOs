#pragma once

#include "typedefs.h"
#include "errno.h"

const char* tostring(int n, int base, char* dest);
int num_len (int base);
int oct2bin(unsigned char *str, int size);
int hex2bin(unsigned char *str, int size);
const char* tostring_inplace(int n, int base);
int string_to_int_dec(const char *s);
const char* tostring_unsigned(uint32_t n, int base, char* dest);

int kulltostr(char* dest, unsigned long long x, unsigned int base, size_t dsize);
int klltostr(char* dest, long long x, unsigned int base, size_t dsize);
int dtostr(char* dest, double value, int precision, size_t dsize);