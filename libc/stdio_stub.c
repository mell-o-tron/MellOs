#include "stdio.h"
#include "errno.h"

int fprintf(FILE* stream, const char* format, ...) {
    errno = ENOSYS;
    (void)stream;
    (void)format;
    return -1;
}
