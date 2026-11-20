#include "kernel_stdio.h"
#include "autoconf.h"
#include "colours.h"
#include "errno.h"
#include "processes.h"
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

extern bool scheduler_active;
extern file_t kernel_stdout_device;
extern file_t kernel_stdin_device;

static void append_char(char** buf, size_t* remaining, char c) {
	if (*remaining > 1) {
		**buf = c;
		(*buf)++;
		(*remaining)--;
	}
}

static void append_str(char** buf, size_t* remaining, const char* s) {
	while (*s) {
		append_char(buf, remaining, *s++);
    }
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

int32_t kvsnprintf(char* buf, size_t size, const char* fmt, va_list va) {
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

__attribute__((format(printf, 1, 2))) int32_t kprintf(char* fmt, ...) {
	char buf[512];
	va_list args;
	va_start(args, fmt);

	kvsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	return kputs(buf);
}

int32_t ksnprintf(char* buf, size_t size, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int32_t ret = kvsnprintf(buf, size, fmt, args);
	va_end(args);
	return ret;
}

static int32_t write_to_stream(file_t* __restrict stream, const char* __restrict s, uint32_t len) {
	if (!s || len == 0)
		return 0;

	WriteLock(&stream->lock, get_current_pid());

	if (stream && stream->ops && stream->ops->write) {
		int written = stream->ops->write(stream, (void*)s, (int)len, 0);
		WriteUnlock(&stream->lock);
		return written < 0 ? -1 : written;
	}

	WriteUnlock(&stream->lock);
	kprint_col("Tried to write to stream but stream does not exist or is broken.\n", ERROR_COLOUR);
	kprint_col((char*)s, ERROR_COLOUR);
	return (int32_t)len;
}

int32_t kfputs(const char* __restrict s, file_t* __restrict stream) {
	if (stream == NULL) {
		return -EINVAL;
	}
	const uint32_t len = kstrlen(s);
	return write_to_stream(stream, s, len);
}
// TODO: Calculate buffer length from format string + args instead of using fixed size buffer
__attribute__((format(printf, 2, 3))) int32_t kfprintf(file_t* stream, const char* fmt, ...) {
	char buf[512];
	va_list args;
	va_start(args, fmt);

	kvsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	return kfputs(buf, stream);
}
/* int32_t kfprintf(file_t* stream, const char* format, ...) { */
/* 	va_list va; */
/* 	va_start(va, format); */
/* 	char buf[256]; */
/* 	const int rval = kvsnprintf(buf, sizeof buf, format, va); */
/* 	va_end(va); */
/* 	if (rval <= 0) */
/* 		return rval; */
/* 	return write_to_stream(stream, buf, (uint32_t)rval); */
/* } */
extern file_t* kstdout;
int32_t kputs(const char* s) {
	return kfputs(s, kstdout);
}
