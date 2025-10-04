/// MellOS - mem.c
/// Edited by assembler-0 on 30/09/25
/// The files in mm/MemOps.c, arch/x86_64/features/x64.h were originally licensed under GPLv2-only for the VoidFrame kernel, but as the sole author, I, assembler-0, hereby release them into the public domain as of 05-10-25 (dd-mm-yy).
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
// to use this compiles with -mserialize
#define _full_mem_prot_end_intel() {\
    __asm__ volatile("mfence; sfence; lfence" ::: "memory");\
    __sync_synchronize();\
    __builtin_ia32_serialize();\ 
}

void* memset(void* dest, unsigned char value, size_t size){
    if (!dest) return NULL;
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

// helper, could later moved for system wide use
static inline int __attribute__((always_inline)) is_aligned16(const void* p){
    return (((unsigned long)p) & 15UL) == 0UL;
}

/* Copy blocks of memory */
// You would use __attribute__((target(...))) for C++ like compile time function overloading targeting older x86 revisions that doesnt have sse2 
void memcp(unsigned char* restrict source, unsigned char* restrict dest, size_t count){
    if (!source || !dest || count == 0) return;
    
    unsigned char *src = source;
    unsigned char *dst = dest;

    if (count < 16){
        return memcpy(source, dest, count);
    }
    
    size_t head = ((unsigned long)dst) & 15UL;
    if (head != 0){
        head = 16 - head; /* how many bytes to copy to reach 16-byte alignment */
        if (head > count) head = count;
        count -= head;
        while (head--) *dst++ = *src++;
        if (count == 0) return;
    }
    
    int src_aligned = is_aligned16(src);
    int dst_aligned = is_aligned16(dst);
    
    /* Use non-temporal stores when destination is aligned to reduce cache pollution for big copies.
    * We'll copy 128 bytes per loop iteration using XMM0..XMM7.
    * The inline asm loop below accepts registers: src -> %0, dst -> %1, count -> %2
    */
    if (count >= 128){
        size_t chunks = count / 128; /* number of 128-byte iterations */
        /* Inline asm loop: load 8 xmm registers from src, store them with MOVNTDQ to dst, update pointers */
        _full_mem_prot_start();
        asm volatile(
            "1:\n\t"
            "movdqu (%%rsi), %%xmm0\n\t"
            "movdqu 16(%%rsi), %%xmm1\n\t"
            "movdqu 32(%%rsi), %%xmm2\n\t"
            "movdqu 48(%%rsi), %%xmm3\n\t"
            "movdqu 64(%%rsi), %%xmm4\n\t"
            "movdqu 80(%%rsi), %%xmm5\n\t"
            "movdqu 96(%%rsi), %%xmm6\n\t"
            "movdqu 112(%%rsi), %%xmm7\n\t"
            /* Use MOVNTDQ if dst is aligned (non-temporal). If src_aligned==1 and dst_aligned==1, we could use MOVDQA loads,
            * but MOVDQU loads are safe & often just as fast on modern CPUs; keep it simple and correct for unaligned src.
            */
            "movntdq %%xmm0, (%%rdx)\n\t"
            "movntdq %%xmm1, 16(%%rdx)\n\t"
            "movntdq %%xmm2, 32(%%rdx)\n\t"
            "movntdq %%xmm3, 48(%%rdx)\n\t"
            "movntdq %%xmm4, 64(%%rdx)\n\t"
            "movntdq %%xmm5, 80(%%rdx)\n\t"
            "movntdq %%xmm6, 96(%%rdx)\n\t"
            "movntdq %%xmm7, 112(%%rdx)\n\t"
            "add $128, %%rsi\n\t"
            "add $128, %%rdx\n\t"
            "dec %%rcx\n\t"
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
                "movdqa (%%rsi), %%xmm0\n\t"
                "movdqa %%xmm0, (%%rdx)\n\t"
                :
                : "S"(src), "D"(dst)
                : "memory", "xmm0"
                );
                } else {
                /* unaligned load, aligned store */
                asm volatile(
                "movdqu (%%rsi), %%xmm0\n\t"
                "movdqa %%xmm0, (%%rdx)\n\t"
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
