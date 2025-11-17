/// MellOS - mem.c (32-bit port)
/// Ported from 64-bit version by assembler-0
/// Public domain as of 05-10-25 (dd-mm-yy)
#include "mem.h"
#include "autoconf.h"
#include "cpu/cpuid.h"
#include "cpu/irq.h"
#include "stddef.h"
#include "stdint.h"

#define _full_mem_prot_start()                                                                     \
	{                                                                                              \
		__sync_synchronize();                                                                      \
		__asm__ volatile("mfence; sfence; lfence" ::: "memory");                                   \
	}
#define _full_mem_prot_end()                                                                       \
	{                                                                                              \
		__asm__ volatile("mfence; sfence; lfence" ::: "memory");                                   \
		__sync_synchronize();                                                                      \
	}

/**
 * Memset as defined in Unix standard. Uses sse2 if it is enabled in compiler flags, cpuid is
 * enabled and cpu supports sse2. Uses the lowest 8 bits of value.
 * @param dest Destination start
 * @param value Value. Gets converted to uint8_t. Uses the lowest 8 bits.
 * @param size How many times to fill the lowest 8 bits of value.
 * @return NULL on failure, dest if value is 0 or success.
 */
__attribute__((section(".low.text"))) void* memset(void* dest, int value, size_t size) {
	if (!dest)
		return NULL;
	if (size == 0)
		return dest;
	uint8_t* dest_low8 = (uint8_t*)dest;

	uint8_t val = (uint8_t)value;

#ifdef CONFIG_CPU_FEAT_SSE2
	if (cpuid_has_sse() && size >= 16) {
		uint32_t val32 = 0x01010101UL * val;

		__asm__ volatile("movd %0, %%xmm0\n"
		                 "pshufd $0, %%xmm0, %%xmm0\n" // Broadcast to all 4 dwords
		                 :
		                 : "r"(val32)
		                 : "xmm0");

		while (size >= 16) {
			__asm__ volatile("movdqu %%xmm0, (%0)" : : "r"(dest_low8) : "memory");
			dest_low8 += 16;
			size -= 16;
		}
	} else {
#endif
		// if (size >= 4) {
		// 	uint32_t val32 = 0x01010101UL * val;
		//
		// 	while (size >= 4 && ((uintptr_t)dest_low8 & 3) == 0) {
		// 		*(uint32_t*)dest_low8 = val32;
		// 		dest_low8 += 4;
		// 		size -= 4;
		// 	}
		// }
#ifdef CONFIG_CPU_FEAT_SSE2
	}
#endif

	// Handle remaining bytes
	while (size--)
		*dest_low8++ = val;
	return dest;
}

// helper, could later moved for system wide use
static inline int __attribute__((always_inline)) is_aligned16(const void* p) {
	return (((unsigned long)p) & 15UL) == 0UL;
}

/* Copy blocks of memory */
void memcp(unsigned char* restrict source, unsigned char* restrict dest, size_t count) {
	if (!source || !dest || count == 0)
		return;

/*
 * This implementation is a lot faster than the old one as it uses SSE2 instructions to copy 16
 * bytes at a time. It first aligns the destination to 16 bytes, then it checks if the source is
 * also aligned. If both are aligned, it uses aligned moves, otherwise it uses unaligned moves.
 * After the SSE2 copy, it copies the remaining bytes that are not a multiple of 16.
 * If SSE2 is not available, it falls back to a 4-byte copy implementation.
 */

// SSE2 implementation
#ifdef CONFIG_CPU_FEAT_SSE2
	if (cpuid_has_sse() && count >= 16) {
		irqflags_t irqf = local_irq_save();
		local_irq_disable();

		// Align dest to 16 bytes
		while (((uintptr_t)dest & 15) != 0 && count > 0) {
			*dest++ = *source++;
			count--;
		}

		// If source is also aligned, use aligned moves
		if (((uintptr_t)source & 15) == 0) {
			while (count >= 16) {
				__asm__ __volatile__("movaps (%%esi), %%xmm0\n"
				                     "movaps %%xmm0, (%%edi)\n"
				                     :
				                     : "S"(source), "D"(dest)
				                     : "xmm0", "memory");
				source += 16;
				dest += 16;
				count -= 16;
			}
		} else // Unaligned source
		{
			while (count >= 16) {
				__asm__ __volatile__("movups (%%esi), %%xmm0\n"
				                     "movaps %%xmm0, (%%edi)\n"
				                     :
				                     : "S"(source), "D"(dest)
				                     : "xmm0", "memory");
				source += 16;
				dest += 16;
				count -= 16;
			}
		}
		__asm__ volatile("sfence" ::: "memory");
		local_irq_restore(irqf);
		return;
	}
#endif

	// 4-byte alignment
	while (count >= 4) {
		*(uint32_t*)dest = *(uint32_t*)source;
		dest += 4;
		source += 4;
		count -= 4;
	}

	// Copy remaining bytes
	while (count > 0) {
		*dest++ = *source++;
		count--;
	}
}

void* memcpy(void* restrict dest, const void* restrict src, uint32_t size) {
	if (size == 0)
		return dest;

	uint8_t* d = (uint8_t*)dest;
	const uint8_t* s = (const uint8_t*)src;

#ifdef CONFIG_CPU_FEAT_SSE2
	if (cpuid_has_sse() && size >= 16) {
		// SSE2 copy using unaligned load/store. Disable IRQs to avoid ISR clobber.
		irqflags_t irqf = local_irq_save();
		local_irq_disable();
		while (size >= 16) {
			__asm__ volatile("movdqu (%1), %%xmm7\n"
			                 "movdqu %%xmm7, (%0)\n"
			                 :
			                 : "r"(d), "r"(s)
			                 : "memory", "xmm7");
			d += 16;
			s += 16;
			size -= 16;
		}
		__asm__ volatile("sfence" ::: "memory");
		local_irq_restore(irqf);
		return d;
	}
#endif

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
				d32[0] = s32[0];
				d32[1] = s32[1];
				d32[2] = s32[2];
				d32[3] = s32[3];
				d32[4] = s32[4];
				d32[5] = s32[5];
				d32[6] = s32[6];
				d32[7] = s32[7];
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
					if (p1[i] < p2[i])
						return -1;
					if (p1[i] > p2[i])
						return 1;
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
		if (*p1 < *p2)
			return -1;
		if (*p1 > *p2)
			return 1;
		p1++;
		p2++;
		size--;
	}
	return 0;
}
