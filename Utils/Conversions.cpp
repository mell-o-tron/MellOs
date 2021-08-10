//"Software version 7.0"


/*********************** FUNCTIONS *************************
* 							     *
* toString: converts int into string - arbitrary base      *
* 							     *
***********************************************************/

#include <string.h>

char toStringRes[128];

const char* toString(int n, int base) {

	char* buffer = toStringRes;
	
	int m = n;
	int i = 0;
    
    if(n < 0){                 // Negative integers
        m = -n;
    }
	
    while(m != 0){
		buffer[i] = (char)((m % base)+ (m % base > 9 ? 55 : 48));
		m = m / base;
		i++;
	}
    
    if(n < 0){
        buffer[i] = '-';
        i++;
    }
    
    if(n == 0){
        buffer[i] = '0';
        i++;
    }
	
	buffer[i] = '\0';
	char revBuf[128];
	//asm volatile("1: jmp 1b");
	return reverse(buffer, revBuf, i-1);
} 

int oct2bin(unsigned char *str, int size) {
    int n = 0;
    unsigned char *c = str;
    while (size-- > 0) {
        n *= 8;
        n += *c - '0';
        c++;
    }
    return n;
}
