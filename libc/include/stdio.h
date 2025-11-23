#pragma once

#include "stddef.h"
#include "stdint.h"
#include "circular_queue.h"
// File structure definition
struct _FILE; typedef struct _FILE FILE;

// File position type
typedef struct _fpos_t {
    uint32_t position;
} fpos_t;

// Constants
#define EOF (-1)
#define BUFSIZ 8192
#define FOPEN_MAX 16
#define FILENAME_MAX 256
#define L_tmpnam 20
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define TMP_MAX 238328

#ifndef NULL
#define NULL ((void*)0)
#endif

// File flags (internal use)
#define _FILE_READ    0x0001 // NOLINT(*-reserved-identifier)
#define _FILE_WRITE   0x0002 // NOLINT(*-reserved-identifier)
#define _FILE_EOF     0x0004 // NOLINT(*-reserved-identifier)
#define _FILE_ERROR   0x0008 // NOLINT(*-reserved-identifier)
#define _FILE_UNBUF   0x0010 // NOLINT(*-reserved-identifier)
#define _FILE_LINEBUF 0x0020 // NOLINT(*-reserved-identifier)

// Standard streams (to be defined in implementation)
#define FD_STDOUT 1
#define FD_STDERR 2
#define FD_STDIN  0

static FILE* stdin;
static FILE* stdout;
static FILE* stderr;

// File operations
FILE* fopen(const char* __restrict filename, const char* __restrict mode)
    __attribute__((nonnull(1, 2)));

FILE* freopen(const char* __restrict filename, const char* __restrict mode, FILE* __restrict stream)
    __attribute__((nonnull(2, 3)));

int fclose(FILE* stream)
    __attribute__((nonnull(1)));

int fflush(FILE* stream);

void setbuf(FILE* __restrict stream, char* __restrict buf)
    __attribute__((nonnull(1)));

int setvbuf(FILE* __restrict stream, char* __restrict buf, int mode, size_t size)
    __attribute__((nonnull(1)));

// Character I/O
int fgetc(FILE* stream)
    __attribute__((nonnull(1)));

char* fgets(char* __restrict buf, int length, FILE* __restrict stream)
    __attribute__((nonnull(1, 3)));

int fputc(int c, FILE* stream)
    __attribute__((nonnull(2)));

int fputs(const char* __restrict s, FILE* __restrict stream)
    __attribute__((nonnull(1, 2)));

int getc(FILE* stream)
    __attribute__((nonnull(1)));

int getchar(void);

char* gets(char* s)
    __attribute__((nonnull(1)))
    __attribute__((deprecated("gets() is unsafe, use fgets() instead")));

int putc(int c, FILE* stream)
    __attribute__((nonnull(2)));

int putchar(int c);

int puts(const char* s)
    __attribute__((nonnull(1)));

int ungetc(int c, FILE* stream)
    __attribute__((nonnull(2)));

// Formatted output
int fprintf(FILE* __restrict stream, const char* __restrict format, ...)
    __attribute__((format(printf, 2, 3)))
    __attribute__((nonnull(1, 2)));

int printf(const char* format, ...)
    __attribute__((format(printf, 1, 2)))
    __attribute__((nonnull(1)));

int sprintf(char* __restrict s, const char* __restrict format, ...)
    __attribute__((format(printf, 2, 3)))
    __attribute__((nonnull(1, 2)));

int vfprintf(FILE* __restrict stream, const char* __restrict format, va_list arg)
    __attribute__((format(printf, 2, 0)))
    __attribute__((nonnull(1, 2)));

int vprintf(const char* format, va_list arg)
    __attribute__((format(printf, 1, 0)))
    __attribute__((nonnull(1)));

int vsprintf(char* __restrict s, const char* __restrict format, va_list arg)
    __attribute__((format(printf, 2, 0)))
    __attribute__((nonnull(1, 2)));

// Formatted input
int fscanf(FILE* __restrict stream, const char* __restrict format, ...)
    __attribute__((format(scanf, 2, 3)))
    __attribute__((nonnull(1, 2)));

int scanf(const char* format, ...)
    __attribute__((format(scanf, 1, 2)))
    __attribute__((nonnull(1)));

int sscanf(const char* __restrict s, const char* __restrict format, ...)
    __attribute__((format(scanf, 2, 3)))
    __attribute__((nonnull(1, 2)));

// Direct I/O
size_t fread(void* __restrict ptr, size_t size, size_t nmemb, FILE* __restrict stream)
    __attribute__((nonnull(1, 4)));

size_t fwrite(const void* __restrict ptr, size_t size, size_t nmemb, FILE* __restrict stream)
    __attribute__((nonnull(1, 4)));

// File positioning
int fgetpos(FILE* __restrict stream, fpos_t* __restrict pos)
    __attribute__((nonnull(1, 2)));

int fseek(FILE* stream, long offset, int whence)
    __attribute__((nonnull(1)));

int fsetpos(FILE* stream, const fpos_t* pos)
    __attribute__((nonnull(1, 2)));

long ftell(FILE* stream)
    __attribute__((nonnull(1)));

void rewind(FILE* stream)
    __attribute__((nonnull(1)));

// Error handling
void clearerr(FILE* stream)
    __attribute__((nonnull(1)));

int feof(FILE* stream)
    __attribute__((nonnull(1)))
    __attribute__((pure));

int ferror(FILE* stream)
    __attribute__((nonnull(1)))
    __attribute__((pure));

void perror(const char* s);

// File operations (filesystem level)
int remove(const char* filename)
    __attribute__((nonnull(1)));

int rename(const char* old, const char* new)
    __attribute__((nonnull(1, 2)));

// Temporary files
FILE* tmpfile(void);

char* tmpnam(char* s);


// C99 additions (not in SUSv2)
int snprintf(char* __restrict s, size_t n, const char* __restrict format, ...)
    __attribute__((format(printf, 3, 4)))
    __attribute__((nonnull(3)));

int vsnprintf(char* __restrict s, size_t n, const char* __restrict format, va_list arg)
    __attribute__((format(printf, 3, 0)))
    __attribute__((nonnull(3)));