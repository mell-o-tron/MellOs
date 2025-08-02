//"Software version 7.0"


/*********************** FUNCTIONS *************************
* 							     *
* toString: converts int into string - arbitrary base      *
* 							     *
***********************************************************/

#include "string.h"
#ifdef VGA_VESA
#include "../drivers/vesa/vesa_text.h"
#else
#include "../drivers/vga_text.h"
#endif
#include "../memory/dynamic_mem.h"
#include "conversions.h"
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

int kulltostr(char* dest, unsigned long long x, unsigned int base, size_t dsize) {
    if (dsize == 0)
        return -EOVERFLOW;
    *dest = '\0';
    if (dsize == 1)
        return -EOVERFLOW;
    const char* digits = "0123456789abcdef";
    if (base < 2 || base > __builtin_strlen(digits))
        return -EINVAL;

    /* 64 bit integers require libgcc */
#ifndef ALLOW_64BIT
    /* gcc should be able to do this operation without libgcc */
    if (x > 0xFFFFFFFF)
        return -ERANGE;
    uint32_t x32 = (uint32_t)x;
#endif

	/* Do the actual conversion */
    char* d = dest;
    do {
        if (dsize == 1)
            break;

        dsize--;
#ifdef ALLOW_64BIT
        *d++ = digits[x % base];
        x /= base;
    while (x);
#else
        *d++ = digits[x32 % base];
        x32 /= base;
    } while (x32);
#endif
    *d = '\0';

    /* Make sure the conversion wasn't cut off by dsize */
#ifdef ALLOW_64BIT
    if (x)
	    return -EOVERFLOW
#else
    if (x32)
        return -EOVERFLOW;
#endif

    /* Now reverse the string */
    d--;
    while (dest < d) {
        char tmp = *dest;
        *dest++ = *d;
        *d-- = tmp;
    }

    return 0;
}

int klltostr(char* dest, long long x, unsigned int base, size_t dsize) {
    if (x < 0) {
        if (dsize == 0)
            return -EOVERFLOW;
        if (--dsize == 0) {
            *dest = '\0';
            return -EOVERFLOW;
        }
        *dest++ = '-';
        x = -x;
    }

    return kulltostr(dest, (unsigned long long)x, base, dsize);
}
