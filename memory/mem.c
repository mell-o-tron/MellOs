#include "../utils/typedefs.h"

void* memset(void* dest, unsigned char val, size_t count){
    /* Indicate failure */
    if (!dest)
        return NULL;

	unsigned char* destC = (unsigned char*)dest;
	int i;
	for (i = 0; i < count; i++)
		destC[i] = val;
	return dest;
}

/* Copy blocks of memory */
void memcp(unsigned char* source, unsigned char* dest, size_t count){
    if (!source || !dest)
        return;

    for (int i = 0; i < count; i++)
        *(dest + i) = *(source + i);
}
