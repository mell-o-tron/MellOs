#pragma once

#ifndef NULL
#define NULL ((void*)0)
#endif
typedef void (*function_type)(void);
typedef struct {
    _Bool is_some;
    int val;

} maybe_int;
typedef maybe_int maybe_void;
#define offsetof(type, member) __builtin_offsetof(type, member)