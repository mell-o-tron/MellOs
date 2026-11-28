#include "cpu/cpuid.h"
#include "autoconf.h"
#include "stdint.h"

#include "cpu/irq.h"
#include "cpu/isr.h"

/**
 * Issue a single request to CPUID. Fits 'intel features', for instance.
 * Note that even if only "eax" and "edx" are of interest, other registers
 * will be modified by the operation, so we need to tell the compiler about it.
 *
 * @param code   The CPUID function code (e.g., 0 for vendor ID, 1 for features)
 * @param a      Pointer to store EAX output
 * @param d      Pointer to store EDX output
 */
#define CPUID(code, a, d)                                                                          \
	do {                                                                                           \
		asm volatile("cpuid" : "=a"(*(a)), "=d"(*(d)) : "a"(code) : "ecx", "ebx");                 \
	} while (0)

/**
 * Issue a complete CPUID request, storing general registers output as an array.
 * Stores results in order: where[0]=EAX, where[1]=EBX, where[2]=ECX, where[3]=EDX
 * Returns EAX output via a separate output parameter.
 *
 * @param code   The CPUID function code
 * @param where  Pointer to uint32_t array of size 4
 * @param result Pointer to store the EAX return value
 */
#define CPUID_STRING(code, where, result)                                                          \
	do {                                                                                           \
		asm volatile("cpuid"                                                                       \
		             : "=a"(*(where)), "=b"(*((where) + 1)), "=c"(*((where) + 2)),                 \
		               "=d"(*((where) + 3))                                                        \
		             : "a"(code));                                                                 \
		if ((result) != NULL) {                                                                    \
			*(result) = (uint32_t)(*(where));                                                      \
		}                                                                                          \
	} while (0)

// /* Example: Get CPU's model number */
// static int get_model(void)
// {
//     int ebx, unused;
//     cpuid(0, unused, ebx, unused, unused);
//     return ebx;
// }

int get_mmx_no_cpuid() {
	asm volatile("cli");

	probing = true;
	probe_type = PROBE_TYPE_MMX;

	asm volatile (
		"xor %%eax, %%eax\n\t"
		"pxor %%mm0, %%mm0\n\t"   // if CPU has no MMX -> #UD
		"emms\n\t"
		:
		:
		: "eax"
	);

	//revert back to normal interrupt handling
	asm volatile("sti");
	if (probe_result) {
		return 1;
	}
	return 0;
}

/* Example: Check for builtin local APIC. */
int cpuid_has_mmx(void) {
#if CONFIG_CPU_FEAT_MMX && CONFIG_CPU_FEAT_CPUID
	unsigned int eax, edx;
	CPUID(1, &eax, &edx);
	return edx & CPUID_FEAT_EDX_MMX;
#elif CONFIG_CPU_FEAT_MMX
	return get_mmx_no_cpuid();
#else
	return 0;
#endif
}
__attribute__((section(".low.text"))) int cpuid_has_sse(void) {
#if CONFIG_CPU_FEAT_SSE && CONFIG_CPU_FEAT_CPUID
	unsigned int eax, edx;
	CPUID(1, &eax, &edx);
	return edx & CPUID_FEAT_EDX_SSE;
#else
	return 0;
#endif
}

enum cpuid_requests {
	CPUID_GETVENDORSTRING,
	CPUID_GETFEATURES,
	CPUID_GETTLB,
	CPUID_GETSERIAL,

	CPUID_INTELEXTENDED = 0x80000000,
	CPUID_INTELFEATURES,
	CPUID_INTELBRANDSTRING,
	CPUID_INTELBRANDSTRINGMORE,
	CPUID_INTELBRANDSTRINGEND,
};