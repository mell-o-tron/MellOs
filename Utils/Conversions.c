//"Software version 7.0"


/*********************** FUNCTIONS *************************
* 							     *
* toString: converts int into string - arbitrary base      *
* 							     *
***********************************************************/

#include <string.h>
#include "../Drivers/VGA_Text.h"

char toStringRes[128];
char revBuf[128];
const char* toString(int n, int base) {
    
    for (int i = 0; i < 128; i++){
        toStringRes[i] = 0;
        revBuf[i] = 0;
    }
    
	char* buffer = toStringRes;
	
	int m = n;
	int i = 0;
    
    if(n < 0){                 // Negative integers
        m = -n;
    }
	
    while(m != 0){
        //kprint("banana");
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
	
	//asm volatile("1: jmp 1b");
    
	return reverse(buffer, revBuf, i);
}

const char* kprintInt(int n) {

	char* buffer = toStringRes;
	int base = 10;
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
	if (n<10){
	    kprint("0");
	    kprint(buffer);
	} else {
	kprint(reverse(buffer,revBuf,i));}
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

int hex2bin(unsigned char *str, int size) {
    int n = 0;
    unsigned char *c = str;
    while(size-- > 0) {
        n *= 16;
        if(n >= '0' && n <= '9') n += *c - '0';
        else if (n >= 'A' && n <= 'F') n += *c - 55;
        c++;
    }
    return n;
}
