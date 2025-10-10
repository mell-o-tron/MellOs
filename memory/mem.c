/// MellOS - mem.c (32-bit port)
/// Ported from 64-bit version by assembler-0
/// Public domain as of 05-10-25 (dd-mm-yy)
#include "../utils/typedefs.h"
#include "../cpu/cpuid.h"


#define _full_mem_prot_start() {\
    __sync_synchronize();\
    __asm__ volatile("mfence; sfence; lfence" ::: "memory");\
}
#define _full_mem_prot_end() {\
    __asm__ volatile("mfence; sfence; lfence" ::: "memory");\
    __sync_synchronize();\
}

void* memset(void* dest, unsigned char value, size_t size){
    if (!dest) return NULL;
    if (size == 0) return dest;
    uint8_t* d = (uint8_t*)dest;
    uint8_t val = (uint8_t)value;
    
    if (cpuid_have_sse() && size >= 16) {
        uint32_t val32 = 0x01010101UL * val;

        __asm__ volatile(
            "movd %0, %%xmm0\n"
            "pshufd $0, %%xmm0, %%xmm0\n"  // Broadcast to all 4 dwords
            :
            : "r"(val32)
            : "xmm0"
        );

        while (size >= 16) {
            __asm__ volatile("movdqu %%xmm0, (%0)" : : "r"(d) : "memory");
            d += 16;
            size -= 16;
        }
    }
    else if (size >= 4) {
        uint32_t val32 = 0x01010101UL * val;

        while (size >= 4 && ((uintptr_t)d & 3) == 0) {
            *(uint32_t*)d = val32;
            d += 4;
            size -= 4;
        }
    }

    // Handle remaining bytes
    while (size--) *d++ = val;
    return dest;
}

// helper, could later moved for system wide use
static inline int __attribute__((always_inline)) is_aligned16(const void* p){
    return (((unsigned long)p) & 15UL) == 0UL;
}

/* Copy blocks of memory */
void memcp(unsigned char* restrict source, unsigned char* restrict dest, size_t count){
    if (!source || !dest || count == 0) return;
    
    unsigned char *src = source;
    unsigned char *dst = dest;

    if (count < 16){
        return memcpy(dest, source, count);
    }
    
    size_t head = ((unsigned long)dst) & 15UL;
    if (head != 0){
        head = 16 - head;
        if (head > count) head = count;
        count -= head;
        while (head--) *dst++ = *src++;
        if (count == 0) return;
    }
    
    int src_aligned = is_aligned16(src);
    int dst_aligned = is_aligned16(dst);
    
    /* Use non-temporal stores when destination is aligned to reduce cache pollution for big copies.
     * We'll copy 128 bytes per loop iteration using XMM0..XMM7.
     * 32-bit uses esi/edi/ecx instead of rsi/rdi/rcx
     */
    if (count >= 128){
        size_t chunks = count / 128;
        _full_mem_prot_start();
        asm volatile(
            "1:\n\t"
            "movdqu (%%esi), %%xmm0\n\t"
            "movdqu 16(%%esi), %%xmm1\n\t"
            "movdqu 32(%%esi), %%xmm2\n\t"
            "movdqu 48(%%esi), %%xmm3\n\t"
            "movdqu 64(%%esi), %%xmm4\n\t"
            "movdqu 80(%%esi), %%xmm5\n\t"
            "movdqu 96(%%esi), %%xmm6\n\t"
            "movdqu 112(%%esi), %%xmm7\n\t"
            "movntdq %%xmm0, (%%edi)\n\t"
            "movntdq %%xmm1, 16(%%edi)\n\t"
            "movntdq %%xmm2, 32(%%edi)\n\t"
            "movntdq %%xmm3, 48(%%edi)\n\t"
            "movntdq %%xmm4, 64(%%edi)\n\t"
            "movntdq %%xmm5, 80(%%edi)\n\t"
            "movntdq %%xmm6, 96(%%edi)\n\t"
            "movntdq %%xmm7, 112(%%edi)\n\t"
            "add $128, %%esi\n\t"
            "add $128, %%edi\n\t"
            "dec %%ecx\n\t"
            "jnz 1b\n\t"
            : /* outputs */
            : /* inputs */ "S"(src), "D"(dst), "c"(chunks)
            : "memory", "xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7"
        );
        
        _full_mem_prot_end();
        
        size_t moved = (count / 128) * 128;
        src += moved;
        dst += moved;
        count -= moved;
    }
    
    while (count >= 16){
        if (is_aligned16(src)){
            /* aligned load + aligned store */
            asm volatile(
                "movdqa (%%esi), %%xmm0\n\t"
                "movdqa %%xmm0, (%%edi)\n\t"
                :
                : "S"(src), "D"(dst)
                : "memory", "xmm0"
            );
        } else {
            /* unaligned load, aligned store */
            asm volatile(
                "movdqu (%%esi), %%xmm0\n\t"
                "movdqa %%xmm0, (%%edi)\n\t"
                :
                : "S"(src), "D"(dst)
                : "memory", "xmm0"
            );
        }
        src += 16;
        dst += 16;
        count -= 16;
    }

    while (count--) *dst++ = *src++;
}

void *memcpy(void * restrict dest, const void * restrict src, uint32_t size)
{
    if (size == 0) return dest;

    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;

    if (cpuid_have_sse() && size >= 16) {
        // SSE2 copy using unaligned load/store. Disable IRQs to avoid ISR clobber.
        irq_flags_t irqf = save_irq_flags();
        cli();
        while (size >= 16) {
            __asm__ volatile(
                "movdqu (%1), %%xmm7\n"
                "movdqu %%xmm7, (%0)\n"
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

    if (size >= 4) {
        // Byte-align destination to 4-byte boundary
        while (((uintptr_t)d & 3) != 0 && size > 0) {
            *d++ = *s++;
            size--;
        }

        // If src is also aligned, we can use fast 32-bit moves
        if (((uintptr_t)s & 3) == 0) {
            uint32_t* d32 = (uint32_t*)d;
            const uint32_t* s32 = (const uint32_t*)s;

            while (size >= 32) {
                d32[0] = s32[0]; d32[1] = s32[1]; d32[2] = s32[2]; d32[3] = s32[3];
                d32[4] = s32[4]; d32[5] = s32[5]; d32[6] = s32[6]; d32[7] = s32[7];
                d32 += 8;
                s32 += 8;
                size -= 32;
            }
            while (size >= 4) {
                *d32++ = *s32++;
                size -= 4;
            }
            d = (uint8_t*)d32;
            s = (const uint8_t*)s32;
        }
    }

    // Handle the remainder
    while (size > 0) {
        *d++ = *s++;
        size--;
    }

    return dest;
}

int memcmp(const void* ptr1, const void* ptr2, uint32_t size) {
    const uint8_t* p1 = (const uint8_t*)ptr1;
    const uint8_t* p2 = (const uint8_t*)ptr2;

    // 32-bit comparison for aligned data
    if (size >= 4 && ((uintptr_t)p1 & 3) == 0 && ((uintptr_t)p2 & 3) == 0) {
        const uint32_t* q1 = (const uint32_t*)p1;
        const uint32_t* q2 = (const uint32_t*)p2;

        while (size >= 4) {
            if (*q1 != *q2) {
                // Found difference, need to find which byte
                p1 = (const uint8_t*)q1;
                p2 = (const uint8_t*)q2;
                for (int i = 0; i < 4; i++) {
                    if (p1[i] < p2[i]) return -1;
                    if (p1[i] > p2[i]) return 1;
                }
            }
            q1++;
            q2++;
            size -= 4;
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
