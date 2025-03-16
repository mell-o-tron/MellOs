//"Software version 7.0"


/*********************** FUNCTIONS *************************
* 							     *
* toString: converts int into string - arbitrary base      *
* 							     *
***********************************************************/

#include "string.h"
#ifdef VGA_VESA
#include "../drivers/vesa_text.h"
#else
#include "../drivers/vga_text.h"
#endif
#include "../memory/dynamic_mem.h"
#include "math.h"


const char* tostring(int n, int base, char* dest) {
    
	char* buffer = dest;
	
	int m = n;
	int i = 0;
    
    if(n < 0) m = -n;
	
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

	reverse(buffer);

	return buffer;
}
const char* tostring_unsigned(uint32_t n, int base, char* dest) {
    
    char* buffer = dest;
    
    uint32_t m = n;
    int i = 0;
    
    while(m != 0){
        buffer[i] = (char)((m % base)+ (m % base > 9 ? 55 : 48));
        m = m / base;
        i++;
    }
    
    if(n == 0){
        buffer[i] = '0';
        i++;
    }
    
    buffer[i] = '\0';

    reverse(buffer);

    return buffer;
}


const char* tostring_inplace(int n, int base) {
    // int size = ceil_log(n, base);
    char* res = kmalloc(20); // Using bad approx, doubling just to be sure

    tostring(n, base, res);

    return res;
}

int num_len (int base){ // TODO

    return 20;
    // return (int)((ceil(log(base, num))+1)*sizeof(char))
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

int string_to_int_dec(const char *s) {
    int sign = 1, result = 0;

    // Skip leading whitespaces
    while (*s == ' ' || *s == '\t' || *s == '\n' ||
           *s == '\r' || *s == '\f' || *s == '\v') {
        s++;
    }

    if (*s == '-' || *s == '+') {
        if (*s == '-')
            sign = -1;
        s++;
    }

    while (*s >= '0' && *s <= '9') {
        result = result * 10 + (*s - '0');
        s++;
    }

    return sign * result;
}
