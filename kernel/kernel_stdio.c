#include "autoconf.h"
#include "stddef.h"
#include "stdio.h"
#include "string.h"

#ifdef CONFIG_GFX_VESA
#include "vesa_text.h"
#else
#include "vga_text.h"
#endif

struct va_wrap {
	va_list va;
};

static void append_char(char** buf, size_t* remaining, char c) {
	if (*remaining > 1) {
		**buf = c;
		(*buf)++;
		(*remaining)--;
	}
}

static void append_str(char** buf, size_t* remaining, const char* s) {
	while (*s)
		append_char(buf, remaining, *s++);
}

static void append_uint32(char** buf, size_t* remaining, uint32_t val, int base, int uppercase,
                          int width, char pad) {
	char tmp[32];
	const char* digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
	int i = 0;

	if (val == 0)
		tmp[i++] = '0';
	else {
		while (val && i < (int)sizeof(tmp)) {
			tmp[i++] = digits[val % base];
			val /= base;
		}
	}

	while (i < width) {
		tmp[i++] = pad;
	}

	while (i--) {
		append_char(buf, remaining, tmp[i]);
	}
}

uint32_t ksnprintf(char* buf, size_t size, const char* fmt, va_list va) {
	struct va_wrap wrap;

	va_copy(wrap.va, va);

	char** out = &buf;
	size_t remaining = size ? size : 1;

	for (const char* p = fmt; *p; p++) {
		if (*p != '%') {
			append_char(out, &remaining, *p);
			continue;
		}

		p++; // skip '%'

		char pad = ' ';
		int width = 0;
		int long_count = 0;
		int uppercase = 0;

		if (*p == '0') {
			pad = '0';
			p++;
		}

		while (*p >= '0' && *p <= '9') {
			width = width * 10 + (*p - '0');
			p++;
		}

		while (*p == 'l') { // 'l' or 'll'
			long_count++;
			p++;
		}

		// --- Conversion specifiers ---
		switch (*p) {
		case 'c':
			append_char(out, &remaining, (char)va_arg(wrap.va, int));
			break;

		case 's': {
			const char* s = va_arg(wrap.va, const char*);
			append_str(out, &remaining, s ? s : "(null)");
			break;
		}

		case 'd':
		case 'i': {
			int v = va_arg(wrap.va, int);
			if (v < 0) {
				append_char(out, &remaining, '-');
				v = -v;
			}
			append_uint32(out, &remaining, v, 10, 0, width, pad);
			break;
		}

		case 'u': {
			uint32_t v =
			    (long_count > 0) ? va_arg(wrap.va, unsigned long) : va_arg(wrap.va, unsigned int);
			append_uint32(out, &remaining, v, 10, 0, width, pad);
			break;
		}

		case 'x':
		case 'X': {
			uppercase = (*p == 'X');
			uint32_t v = 0;
			if (long_count >= 1)
				v = va_arg(wrap.va, unsigned long);
			else
				v = va_arg(wrap.va, unsigned int);

			append_str(out, &remaining, "0x");
			append_uint32(out, &remaining, v, 16, uppercase, width, pad);
			break;
		}

		case '%':
			append_char(out, &remaining, '%');
			break;

		default:
			append_char(out, &remaining, '%');
			append_char(out, &remaining, *p);
			break;
		}
	}

	va_end(wrap.va);
	append_char(out, &remaining, '\0');
	return (int)(*out - buf);
}

__attribute__((format(printf, 1, 2))) uint32_t kprintf(char* fmt, ...) {
	char buf[512];
	va_list args;
	va_start(args, fmt);

	ksnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	return kprint(buf);
}