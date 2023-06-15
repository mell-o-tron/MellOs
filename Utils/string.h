
#ifndef STRING_H
#define STRING_H
#include "Typedefs.h"
const char* reverse(const char* str, char* buffer, int len);
int strlen(const char* s);
bool StringsEqu(const char* s, const char* t);
bool StringStartsWith(const char* s, const char* t);
int strFindChar(const char* s, char c);
const char* strDecapitate(const char* s, int n);
char * strcpy(char *strDest, const char *strSrc);
#endif
