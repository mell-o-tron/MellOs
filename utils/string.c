/*********************** FUNCTIONS ********************************
* reverse: reverses a string                                      *
* strlen: returns length of a string                              *
* StringsEqu: returns true if strings are equal (max len 80)      *
* StringStartsWith: returns true if s starts with t (max len 80)  *
******************************************************************/

#include "assert.h"
#include "typedefs.h"
#include "../memory/dynamic_mem.h"

uint32_t strlen(const char* s){
	uint32_t res;
	for(res = 0; s[res] != 0; res++);
	return res;
}

void reverse(char s[])
{
    uint32_t length = strlen(s) ;
    uint32_t c, i, j;

    for (i = 0, j = length - 1; i < j; i++, j--){
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}


uint32_t strcmp(const char* s1, const char* s2)
{
    while(*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}


char * strcpy(char *strDest, const char *strSrc)
{
    assert(strDest!=NULL && strSrc!=NULL);
    char *temp = strDest;
    while((*strDest++=*strSrc++) != '\0');
    return temp;
}


bool string_starts_with(char *s, char *prefix) {
    while (*prefix)
        if (*s++ != *prefix++)
            return false;
    return true;
}

char* str_decapitate(char *s, uint32_t n) {
    size_t len = strlen(s);
    char* res = kmalloc(len - n + 1);

    if (n >= len) res[0] = 0;
    else {
        for (size_t i = 0; i <= len - n; i++)
            res[i] = s[i + n];
    }

    return res;
}
