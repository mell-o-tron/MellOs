/// MellOS - mem.c
/// Edited by assembler-0 on 30/09/25
/// NOTE: the proposed change is taken from the VoidFrame kernel by assembler-0 licensed under GPLv2.0-only
#include "../utils/typedefs.h"
#include "../cpu/cpuid.h"

void* memset(void* dest, unsigned char value, size_t size){
    // ASSERT(dest != NULL)
    if (size == 0) return;
    uint8_t* d = (uint8_t*)dest;
    uint8_t val = (uint8_t)value
    if (cpuid_have_sse() && size >= 16) { // Add function for sse2 detection, though a modern processor should have both
        uint64_t val64 = 0x0101010101010101ULL * val;

        __asm__ volatile(
            "movq %0, %%xmm0\n"
            "punpcklqdq %%xmm0, %%xmm0\n"
            :
            : "r"(val64)
            : "xmm0"
        );

        while (size >= 16) {
            __asm__ volatile("movdqu %%xmm0, (%0)" : : "r"(d) : "memory");
            d += 16;
            size -= 16;
        }
    }
    else if (size >= 8) {
        uint64_t val64 = 0x0101010101010101ULL * val;

        while (size >= 8 && ((uintptr_t)d & 7) == 0) {
            *(uint64_t*)d = val64;
            d += 8;
            size -= 8;
        }
    }

    // Handle remaining bytes
    while (size--) *d++ = val;
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

	const char* restrict final = source + count;
	while (source < final) {
		*(uint32_t*)dest = *(uint32_t*)source;
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

void *memcpy(void * restrict dest, const void * restrict src, uint64_t size)
{
    //ASSERT(dest != NULL && src != NULL);

    if (size == 0) return dest;

    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;

	if (cpuid_have_sse() && size >= 16) {
        // SSE2 copy using unaligned load/store. Disable IRQs to avoid ISR clobber.
        irq_flags_t irqf = save_irq_flags(); // if you do not have this, tell me.
        cli();
        while (size >= 16) {
            __asm__ volatile(
                "movdqu (%1), %%xmm7\n"   // Unaligned read from src
                "movdqu %%xmm7, (%0)\n"   // Unaligned write to dest
                :
                : "r"(d), "r"(s)
                : "memory", "xmm7"
            );
            d += 16;
            s += 16;
            size -= 16;
        }
        __asm__ volatile("sfence" ::: "memory");
        restore_irq_flags(irqf);
    }

    if (size >= 8) {
        // Byte-align destination 
        while (((uintptr_t)d & 7) != 0 && size > 0) {
            *d++ = *s++;
            size--;
        }

        // If src is also aligned, we can use fast 64-bit moves
        if (((uintptr_t)s & 7) == 0) {
            uint64_t* d64 = (uint64_t*)d;
            const uint64_t* s64 = (const uint64_t*)s;

            while (size >= 64) {
                d64[0] = s64[0]; d64[1] = s64[1]; d64[2] = s64[2]; d64[3] = s64[3];
                d64[4] = s64[4]; d64[5] = s64[5]; d64[6] = s64[6]; d64[7] = s64[7];
                d64 += 8;
                s64 += 8;
                size -= 64;
            }
            while (size >= 8) {
                *d64++ = *s64++;
                size -= 8;
            }
            d = (uint8_t*)d64;
            s = (const uint8_t*)s64;
        }
    }

    // Handle the remainder
    while (size > 0) {
        *d++ = *s++;
        size--;
    }

    return dest;
}

int memcmp(const void* ptr1, const void* ptr2, uint64_t size) {
    const uint8_t* p1 = (const uint8_t*)ptr1;
    const uint8_t* p2 = (const uint8_t*)ptr2;

    // 64-bit comparison for aligned data
    if (size >= 8 && ((uintptr_t)p1 & 7) == 0 && ((uintptr_t)p2 & 7) == 0) {
        const uint64_t* q1 = (const uint64_t*)p1;
        const uint64_t* q2 = (const uint64_t*)p2;

        while (size >= 8) {
            if (*q1 != *q2) {
                // Found difference, need to find which byte
                p1 = (const uint8_t*)q1;
                p2 = (const uint8_t*)q2;
                for (int i = 0; i < 8; i++) {
                    if (p1[i] < p2[i]) return -1;
                    if (p1[i] > p2[i]) return 1;
                }
            }
            q1++;
            q2++;
            size -= 8;
        }
        p1 = (const uint8_t*)q1;
        p2 = (const uint8_t*)q2;
    }

    // Compare remaining bytes
    while (size > 0) {
        if (*p1 < *p2) return -1;
        if (*p1 > *p2) return 1;
        p1++;
        p2++;
        size--;
    }
    return 0;
}
