#include "unistd.h"
#include "stdint.h"
#include "conversions.h"
#include "string.h"
#include "stddef.h"
#include "limits.h"
#include "stdio.h"
#include "errno.h"
#include "mem.h"

#include <mellos/kernel/stdio_devices.h>

union arg {
    long long ll;
    unsigned long long ull;
    const char *s;
    const int32_t *ws;
    void *p;
    double d;  // Added for float support
};

struct va_wrap {
    va_list va;
};

enum fmt_flags {
    FMT_PLUS = (1 << 0),
    FMT_LEFT = (1 << 1),
    FMT_ZEROPAD = (1 << 2),
    FMT_SPACE = (1 << 3),
    FMT_SPECIAL = (1 << 4),

    FMT_TYPE_CHAR = (1 << 5),
    FMT_TYPE_STR = (1 << 6),

    FMT_INT_B2 = (1 << 7),
    FMT_INT_B8 = (1 << 8),
    FMT_INT_B16 = (1 << 9),

    FMT_INT_SIGNED = (1 << 10),
    FMT_INT_UPPER = (1 << 11),

    FMT_TYPE_FLOAT = (1 << 12)
};

struct fmt {
    unsigned int flags;
    int field_width, precision;
    char qualifier;
};

static unsigned int get_flags(const char **fmt) {
    unsigned int flags = 0;
    while (1) {
        switch (**fmt) {
        case '+':
            flags |= FMT_PLUS;
            break;
        case '-':
            flags |= FMT_LEFT;
            break;
        case '0':
            flags |= FMT_ZEROPAD;
            break;
        case ' ':
            flags |= FMT_SPACE;
            break;
        case '#':
            flags |= FMT_SPECIAL;
            break;
        default:
            return flags;
        }

        (*fmt)++;
    }
}

static int get_int(const char **fmt, struct va_wrap* va) {
    if (**fmt == '*') {
        (*fmt)++;
        return va_arg(va->va, int);
    }

    int ret = 0;
    while (**fmt >= '0' && **fmt <= '9') {
        ret = ret * 10 + **fmt - '0';
        (*fmt)++;
    }
    return ret;
}

static void parse_fmt(const char **fmt, struct va_wrap* va, struct fmt *spec) {
    spec->flags = get_flags(fmt);

    spec->field_width = get_int(fmt, va);
    if (spec->field_width < 0) {
        spec->field_width = -spec->field_width;
        spec->flags |= FMT_LEFT;
    }

    if (spec->flags & FMT_LEFT)
        spec->flags &= ~FMT_ZEROPAD;

    /* Handle %.xxy specifiers */
    spec->precision = -1;
    if (**fmt == '.') {
        (*fmt)++;
        spec->precision = get_int(fmt, va);
        if (spec->precision >= 0 && spec->flags & FMT_ZEROPAD) {
            spec->field_width = 0;
            spec->flags &= ~FMT_ZEROPAD;
        }
    }

    spec->qualifier = -1;
    switch (**fmt) {
    case 'h':
    case 'l':
        spec->qualifier = **fmt;
        (*fmt)++;
        if (spec->qualifier == **fmt) {
            spec->qualifier -= 'a' - 'A';
            (*fmt)++;
        }
        break;
    case 'z':
        spec->qualifier = **fmt;
        (*fmt)++;
        break;
    }
}

static int get_arg(struct fmt *spec, struct va_wrap* va, char c, union arg *arg) {
    switch (c) {
    case 'c':
        if (spec->qualifier == 'l') {
            arg->ll = va_arg(va->va, int32_t);
        } else {
            arg->ll = (char)va_arg(va->va, int);
        }
        spec->flags |= FMT_TYPE_CHAR;
        break;
    case 's':
        arg->p = va_arg(va->va, void *);
        spec->flags |= FMT_TYPE_STR;
        break;
    case 'p':
        arg->p = va_arg(va->va, void *);
        spec->flags |= FMT_SPECIAL | FMT_INT_B16;
        spec->precision = sizeof(void *) * 2;
        spec->qualifier = 'z'; /* Make sure do_int doesn't cast the pointer to a lower size */
        break;
    case 'f':
    case 'F':
        arg->d = va_arg(va->va, double);
        spec->flags |= FMT_TYPE_FLOAT;
        break;
    case 'd':
    case 'i':
        spec->flags |= FMT_INT_SIGNED;
        __attribute__((fallthrough));
    case 'u':
        spec->flags &= ~FMT_SPECIAL;
        __attribute__((fallthrough));
    case 'X':
        spec->flags |= FMT_INT_UPPER;
        __attribute__((fallthrough));
    case 'x':
    case 'o':
    case 'b':
        if (c == 'x' || c == 'X')
            spec->flags |= FMT_INT_B16;
        else if (c == 'o')
            spec->flags |= FMT_INT_B8;
        else if (c == 'b')
            spec->flags |= FMT_INT_B2;

        switch (spec->qualifier) {
        case 'h':
            arg->ll = (short)va_arg(va->va, int);
            break;
        case 'H':
            arg->ll = (char)va_arg(va->va, int);
            break;
        case 'l':
            arg->ll = va_arg(va->va, long);
            break;
        case 'L':
            arg->ll = va_arg(va->va, long long);
            break;
        case 'z':
            arg->ll = va_arg(va->va, ssize_t);
            break;
        default:
            arg->ll = va_arg(va->va, int);
            break;
        }
        break;
    case '%':
        spec->flags |= FMT_TYPE_CHAR;
        arg->ll = '%';
        break;
    default:
        return -1;
    }

    return 0;
}

/* Write one character to a buffer, dest and dsize are adjusted after the
 * operation */
static inline void write_one(char **dest, char c, size_t *dsize) {
    if (*dsize <= 1)
        return;

    **dest = c;
    (*dest)++;
    (*dsize)--;
}

/* Write several characters to a buffer, dest and dsize are adjusted after the
 * operation */
static inline void write_many(char **dest, const char *str, size_t count, size_t *dsize) {
    while (count--)
        write_one(dest, *str++, dsize);
}

/* Output a character to a buffer */
static long long do_char_output(struct fmt *spec, char **dest, size_t *dsize, union arg *arg) {
    if (spec->qualifier == 'l' && (arg->ll < CHAR_MIN || arg->ll > CHAR_MAX))
        return -1;

    int spaces = spec->field_width == 0 ? 0 : spec->field_width - 1;

    /* Right pad first */
    if (!(spec->flags & FMT_LEFT) && spec->field_width > 0) {
        spec->field_width--;
        while (spec->field_width) {
            write_one(dest, ' ', dsize);
            spec->field_width--;
        }
    }

    write_one(dest, (char)arg->ll, dsize);

    /* Then left pad */
    if (spec->field_width > 0) {
        spec->field_width--;
        while (spec->field_width) {
            write_one(dest, ' ', dsize);
            spec->field_width--;
        }
    }

    /* Check for overflow then return */
    if (spaces + 1 > INT_MAX)
        return -1;

    return spaces + 1;
}

/* Handle outputting a string to a buffer */
static long long do_str_output(struct fmt *spec, char **dest, size_t *dsize, union arg *arg) {
    int len;
    if (!arg->p) {
        len = strlen("(null)");
        if (spec->qualifier == 'l')
            arg->ws = (const int32_t *)L"(null)";
        else
            arg->s = "(null)";
    } else if (spec->qualifier == 'l') {
        len = 0;
        while (arg->ws[len]) {
            if (arg->ws[len] < CHAR_MIN || arg->ws[len] > CHAR_MAX)
                return -1;
            len++;
        }
    } else {
        size_t l = strlen(arg->s);
        if (l > INT_MAX)
            return -1;
        len = l;
    }

    /* Check how many spaces we need for padding */
    int spaces = 0;
    if (len >= spec->field_width) {
        spec->field_width = 0;
    } else {
        spec->field_width -= len;
        spaces = spec->field_width;
    }

    /* Right pad first */
    if (!(spec->flags & FMT_LEFT)) {
        while (spec->field_width) {
            write_one(dest, ' ', dsize);
            spec->field_width--;
        }
    }

    /* Now write the string to the destination */
    if (spec->qualifier == 'l') {
        while (*arg->ws)
            write_one(dest, *arg->ws++, dsize);
    } else {
        write_many(dest, arg->s, len, dsize);
    }

    /* Now left pad if needed */
    while (spec->field_width) {
        write_one(dest, ' ', dsize);
        spec->field_width--;
    }

    return len + spaces;
}

/* Handle outputting a float to a buffer */
static long long do_float_output(struct fmt *spec, char **dest, size_t *dsize, union arg *arg) {
    char conversion[64];

    // Initialize buffer
    memset(conversion, 0, sizeof(conversion));

    // Call dtostr
    int result = dtostr(conversion, arg->d, (spec->precision < 0 ? 6 : spec->precision), sizeof(conversion));
    if (result < 0) {
        return -1;
    }

    int conversion_len = strlen(conversion);
    if (conversion_len == 0) {
        return -1;
    }

    /* Handle field width and padding */
    int spaces = 0;
    if (conversion_len >= spec->field_width) {
        spec->field_width = 0;
    } else {
        spec->field_width -= conversion_len;
        spaces = spec->field_width;
    }

    /* Right pad first (unless left-aligned) */
    if (!(spec->flags & FMT_LEFT)) {
        while (spec->field_width) {
            write_one(dest, ' ', dsize);
            spec->field_width--;
        }
    }

    /* Write the conversion */
    write_many(dest, conversion, conversion_len, dsize);

    /* Left pad if needed */
    while (spec->field_width) {
        write_one(dest, ' ', dsize);
        spec->field_width--;
    }

    return conversion_len + spaces;
}

static int do_int(char *dest, char qualifier, unsigned long long x, unsigned int base, size_t dsize) {
    /* This must be done, otherwise there will be errors when x >
     * TYPE_WIDTH_SIGNED_MAX */
    switch (qualifier) {
    case -1:
        x = (unsigned int)x;
        break;
    case 'h':
        x = (unsigned short)x;
        break;
    case 'H':
        x = (unsigned char)x;
        break;
    case 'l':
        x = (unsigned long)x;
        break;
    case 'L':
        break;
    case 'z':
        x = (size_t)x;
        break;
    default:
        return -EINVAL;
    }

    return kulltostr(dest, x, base, dsize);
}

/* Handle converting an outputting an integer to a string buffer */
static long long do_int_output(struct fmt *spec, char **dest, size_t *dsize, union arg *arg) {
    char conversion[sizeof(long long) * 8 + 1];

    /* First get the base for the format specifier */
    unsigned int base = 10;
    if (spec->flags & FMT_INT_B2)
        base = 2;
    else if (spec->flags & FMT_INT_B8)
        base = 8;
    else if (spec->flags & FMT_INT_B16)
        base = 16;

    /* Now check the sign */
    char sign = '\0';
    if (spec->flags & FMT_INT_SIGNED) {
        if (arg->ll < 0) {
            sign = '-';
            arg->ll = -arg->ll;
        } else if (spec->flags & FMT_PLUS) {
            sign = '+';
        } else if (spec->flags & FMT_SPACE) {
            sign = ' ';
        }
    }
    int sign_len = sign ? 1 : 0;

    /* The special flag will control whether or not the radix is printed */
    const char *radix = NULL;
    if (spec->flags & FMT_SPECIAL) {
        if (base == 2)
            radix = "0b";
        else if (base == 8)
            radix = "0";
        else if (base == 16)
            radix = "0x";
    }
    int radix_len = radix ? strlen(radix) : 0;

    /* Now convert the integer to a string */
    int err =
        do_int(conversion, spec->qualifier, arg->ull, base, sizeof(conversion));
    if (err)
        return -1;
    int conversion_len = strlen(conversion);
    if (base == 16 && spec->flags & FMT_INT_UPPER) {
        char *conv = conversion;
        while (*conv) {
            if (*conv >= 'a' && *conv <= 'f')
                *conv -= 'a' - 'A';
            conv++;
        }
    }

    /* Now handle zeropad and field widths */
    int pad = 0;
    if (spec->flags & FMT_ZEROPAD && spec->field_width >= conversion_len) {
        spec->field_width -= conversion_len;
        pad = spec->field_width;
    } else if (spec->field_width >= conversion_len + radix_len) {
        spec->field_width -= conversion_len + radix_len;
        pad = spec->field_width;
    } else {
        spec->field_width = 0;
    }

    /* Now either zeropad, or pad with spaces. The radix and sign must be
     * written first if zeropadding */
    if (spec->flags & FMT_ZEROPAD) {
        if (sign) {
            write_one(dest, sign, dsize);
            sign = 0; /* Set to zero so it's not written again */
        }

        /* No need to check the pointer here, since radix_len will be zero, so
         * no null dereference */
        write_many(dest, radix, radix_len, dsize);
        radix = NULL; /* Set to NULL so that it isn't written again */

        /* Now do the actual zeropad */
        while (spec->field_width) {
            write_one(dest, '0', dsize);
            spec->field_width--;
        }
    } else if (!(spec->flags & FMT_LEFT)) {
        while (spec->field_width) {
            write_one(dest, ' ', dsize);
            spec->field_width--;
        }
    }

    if (sign)
        write_one(dest, sign, dsize);
    if (radix)
        write_many(dest, radix, radix_len, dsize);

    /* This is similar to zeropad, but can be used with space padding too */
    int precision_pad = 0;
    if (spec->precision != -1) {
        if (spec->precision >= conversion_len) {
            spec->precision -= conversion_len;
            precision_pad = spec->precision;
        }
        while (spec->precision--)
            write_one(dest, '0', dsize);
    }

    /* Now write the conversion */
    write_many(dest, conversion, conversion_len, dsize);

    /* Now left pad if needed */
    while (spec->field_width) {
        write_one(dest, ' ', dsize);
        spec->field_width--;
    }

    return conversion_len + radix_len + pad + sign_len + precision_pad;
}

/* Write the output from a format specifier to a buffer */
static inline long long do_output(struct fmt *spec, char **dest, size_t *dsize, union arg *arg) {
    if (spec->flags & FMT_TYPE_CHAR)
        return do_char_output(spec, dest, dsize, arg);
    else if (spec->flags & FMT_TYPE_STR)
        return do_str_output(spec, dest, dsize, arg);
    else if (spec->flags & FMT_TYPE_FLOAT)
        return do_float_output(spec, dest, dsize, arg);
    return do_int_output(spec, dest, dsize, arg);
}

int vsnprintf(char *dest, size_t dsize, const char *fmt, va_list va) {
    struct va_wrap wrap;
    va_copy(wrap.va, va);

    int char_count = 0;
    while (*fmt) {
        if (*fmt != '%') {
            if (dsize > 1) {
                *dest++ = *fmt;
                dsize--;
            }
            fmt++;
            char_count++;
            continue;
        }

        fmt++;

        struct fmt spec;
        union arg arg;

        parse_fmt(&fmt, &wrap, &spec);
        if (get_arg(&spec, &wrap, *fmt++, &arg))
            goto fail;

        /* Now for writing to output, if the value is negative it's an error */
        long long add = do_output(&spec, &dest, &dsize, &arg);
        if (add < 0)
            goto fail;
        if (__builtin_add_overflow(char_count, add, &char_count))
            goto fail;
    }

    va_end(wrap.va);
    if (dsize)
        *dest = '\0';
    return char_count;
fail:
    va_end(wrap.va);
    return -1;
}

int snprintf(char *dest, size_t dsize, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    const int ret = vsnprintf(dest, dsize, fmt, va);
    va_end(va);
    return ret;
}

// todo: formatted kernel printing and some kernel log

__attribute__((format(printf, 1, 2)))
int printf(const char* s, ...) {
    va_list va;
    va_start(va, s);
	char buf [256];
    const int rval = vsnprintf(buf, sizeof buf, s, va);
    va_end(va);
    syscall_write(FD_STDOUT, buf, rval);
    //kprint(buf);
    return rval;
}

int fputs(const char* __restrict s, FILE* __restrict stream) {
	const uint32_t len = strlen(s); // todo return how many bytes written from int
	syscall_write(stream->fd, s, len);
    return (int32_t) len;
}

int fprintf(FILE* stream, const char* format, ...) {
	va_list va;
	va_start(va, format);
	char buf [256];
	const int rval = vsnprintf(buf, sizeof buf, format, va);
	va_end(va);
	syscall_write(stream->fd, buf, rval);
    errno = ENOSYS;
    return -1;
}
