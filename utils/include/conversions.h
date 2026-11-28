#pragma once

#include "stdint.h"

const char* tostring(int32_t n, int32_t base, char* dest);
int32_t num_len(int32_t base);
int32_t oct2bin(uint8_t *str, int32_t size);
int32_t hex2bin(uint8_t *str, int32_t size);
const char* tostring_inplace(int32_t n, int32_t base);
int32_t string_to_int_dec(const char *s);
const char* tostring_unsigned(uint32_t n, int32_t base, char* dest);

int kulltostr(char* dest, uint64_t x, uint32_t base, size_t dsize);
int klltostr(char* dest, int64_t x, unsigned int base, size_t dsize);
int dtostr(char* dest, double value, int precision, size_t dsize);