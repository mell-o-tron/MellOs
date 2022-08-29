#ifndef SHELLFUNCTIONS_H
#define SHELLFUNCTIONS_H

#include "../Utils/Typedefs.h"

typedef struct {
    void (*fptr)(const char *);
    char* alias;
    char* help;
} __attribute__ ((packed)) shellfunction;

shellfunction* TryGetCMD(char* cmdbuf);
int GetCMDLength();

#endif
