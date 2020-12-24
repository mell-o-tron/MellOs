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
	while(m != 0){
		buffer[i] = (char)((m % base)+ (m % base > 9 ? 55 : 48));
		m = m / base;
		i++;
	}
	buffer[i] = '\0';
	char revBuf[128];
	//asm volatile("1: jmp 1b");
	return reverse(buffer, revBuf, i-1);
} 

