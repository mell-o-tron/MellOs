/*********************** FUNCTIONS ********************************
* reverse: reverses a string                                      *
* strLen: returns length of a string                              *
* StringsEqu: returns true if strings are equal (max len 80)      *
* StringStartsWith: returns true if s starts with t (max len 80)  *
******************************************************************/
#include "../Memory/mem.h"

const char* reverse(const char* str, char* buffer, int len){
	int i;
	for (i = 0; i <= len; i++){
		buffer[i] = str[len - i];
	}
	buffer[i] = '\0';
	return buffer;
}

int strLen(const char* s){
	int res;
	for(res = 0; s[res] != 0; res++);
	return res;
}


bool StringsEqu(const char* s, const char* t){
	const char* g = strLen(s) > strLen(t)? s : t;
	
	bool res = true;
	int i = 0;
	do{
		if(s[i] != t[i] || i > 80){
			res = false;
			break;
		}
		else i++;
	}while(g[i] != 0);
	return res;
}

bool StringStartsWith(const char* s, const char* t){
    const char* g = t;
	
	bool res = true;
	int i = 0;
	do{
		if(s[i] != t[i] || i > 80){
			res = false;
			break;
		}
		else i++;
	}while(g[i] != 0);
	return res;
    
}


int strFindChar(const char* s, char c){
    int i = 0;
	do{
		if(s[i] == c || i > 80){
			break;
		}
		else i++;
	}while(s[i] != 0);
    return(s[i] == 0 || i > 80 ? -1 : i);
}


const char* strDecapitate(const char* s, int n){            // Uses Dynamic Memory Allocation, be careful.
    int newLen = strLen(s) - n;
    if(newLen <= 0) return ""; 
    char* t = (char*)kmalloc(sizeof(char) * (newLen + 1));
    for(int i = 0; i < newLen; i++)
        t[i] = s[i + n];
    return t;
}
