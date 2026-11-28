#ifndef TYPEDEF_H
#define TYPEDEF_H

/* Sanity check, this should not error */
#ifndef __ILP32__
#error "types will not work, ilp32 not used"
#endif /* __ILP32__ */

typedef long int intptr_t;
typedef unsigned long int uintptr_t;

typedef long int ptrdiff_t;


#define low_16(address) (uint16_t)((address) & 0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)
#define low_8(address) (uint8_t)((address) &  0xFF)
#define high_8(address) (uint8_t)(((address) >> 8) & 0xFF)

#define NULL ((void*)0)

typedef struct {
    _Bool is_some;
    int val;

} maybe_int;

typedef maybe_int maybe_void;

typedef void (*function_type)(void);

typedef __builtin_va_list va_list;

#define va_start(v, p) __builtin_va_start(v, p)
#define va_end(v) __builtin_va_end(v)
#define va_arg(v, t) __builtin_va_arg(v, t)
#define va_copy(d, s) __builtin_va_copy(d, s)

#endif
