#pragma once
#include "stdint.h"

#ifndef NULL
#define NULL ((void*)0)
#endif
typedef void (*function_type)(void);
#define offsetof(type, member) __builtin_offsetof(type, member)