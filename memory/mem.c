#include "stdint.h"
#include "stddef.h"
#include "cpu/cpuid.h"
#include "string.h"

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
void memcp(unsigned char* restrict source, unsigned char* restrict dest, size_t count){
    if (!source || !dest)
        return;


	// TODO: Need to make 4byte alignment considerations
	// This implementation should be faster than the one below
	// It first copies bytes until the number of bytes to copy is a multiple of 4
	// Then it copies 4 bytes at a time
	
	while (count % 4 != 0)
	{
		*dest = *source;
		dest++;
		source++;
		count--;
	}

	const unsigned char* restrict final = source + count;
	while (source < final) {
		*(uint32_t*)dest = *(const uint32_t*)source;
		dest += 4;
		source += 4;
	}

	// /* Copy 4 bytes at a time */
	// if (count / 4 > 0){
	// 	for (size_t i = 0; i < count / 4; i++)
	// 		*(uint32_t*)(dest + i * 4) = *(uint32_t*)(source + i * 4);
	// 	count = count % 4;
	// }

    // for (size_t i = 0; i < count; i++)
    //     *(dest + i) = *(source + i);
}

void *memcpy(void * restrict to, const void * restrict from, unsigned int n)
{
	if(cpuid_has_sse())
	{
		int i;
		for(i=0; i<n/16; i++)
		{
			__asm__ __volatile__ ("movups (%0), %%xmm0\n" "movntdq %%xmm0, (%1)\n"::"r"(from), "r"(to) : "memory");

			from += 16;
			to += 16;
		}
	}
	else if(n&15 && cpuid_has_mmx())
	{
		n = n&15;
		int i;
		for(i=0; i<n/8; i++)
		{
			__asm__ __volatile__ ("movq (%0), %%mm0\n" "movq %%mm0, (%1)\n"::"r"(from), "r"(to):"memory");
			from += 8;
			to += 8;
		}
	}
	if(n & 7)
	{
		n = n&7;

		int d0, d1, d2;
		__asm__ __volatile__(
		"rep ; movsl\n\t"
		"testb $2,%b4\n\t"
		"je 1f\n\t"
		"movsw\n"
		"1:\ttestb $1,%b4\n\t"
		"je 2f\n\t"
		"movsb\n"
		"2:"
		: "=&c" (d0), "=&D" (d1), "=&S" (d2)
		:"0" (n/4), "q" (n),"1" ((long) to),"2" ((long) from)
		: "memory");
	}
	return (to);
}