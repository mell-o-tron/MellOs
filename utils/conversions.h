#pragma once
const char* tostring(int n, int base, char* dest);
int num_len (int base);
int oct2bin(unsigned char *str, int size);
int hex2bin(unsigned char *str, int size);
const char* tostring_inplace(int n, int base);
int string_to_int_dec(const char *s);
const char* tostring_unsigned(uint32_t n, int base, char* dest);