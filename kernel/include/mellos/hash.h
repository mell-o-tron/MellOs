#pragma once
#include "stdint.h"

uint32_t hash_djb2(const void* target, size_t length);

uint32_t hash_djb2_string(const char* string);