#ifndef TYPEDEF_H
#define TYPEDEF_H

/* Sanity check, this should not error */
#ifndef __ILP32__
#error "types will not work, ilp32 not used"
#endif /* __ILP32__ */

#define SCHAR_MAX 0x7f
#define SCHAR_MIN (-SCHAR_MAX - 1)
#define UCHAR_MAX 0xffU
#define CHAR_MIN SCHAR_MIN
#define CHAR_MAX SCHAR_MAX
#define USHRT_MAX 0xffffU
#define SHRT_MAX 0x7fff
#define SHRT_MIN (-SHRT_MAX - 1)
#define UINT_MAX 0xffffffffU
#define INT_MAX 0x7fffffff
#define INT_MIN (-INT_MAX - 1)
#define ULONG_MAX 0xffffffffUL
#define LONG_MAX 0x7fffffffL
#define LONG_MIN (-LONG_MAX - 1)
#define ULLONG_MAX 0xffffffffffffffffULL
#define LLONG_MAX 0x7fffffffffffffffLL
#define LLONG_MIN (-LLONG_MAX - 1)

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef unsigned long size_t;
typedef long ssize_t;
typedef unsigned long long uint64_t;

typedef long int intptr_t;
typedef unsigned long int uintptr_t;

typedef long int ptrdiff_t;

typedef uint8_t Colour;
typedef union {
    struct {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    };
    uint32_t val;
} VESA_Colour;

typedef uint8_t bool;
#define true    1
#define false   0

#define low_16(address) (uint16_t)((address) & 0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)
#define low_8(address) (uint8_t)((address) &  0xFF)
#define high_8(address) (uint8_t)(((address) >> 8) & 0xFF)

#define NULL ((void*)0)

typedef struct {
    bool is_some;
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
