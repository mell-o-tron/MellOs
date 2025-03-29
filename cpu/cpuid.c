#include "cpuid.h"
#include "../utils/typedefs.h"

/** issue a single request to CPUID. Fits 'intel features', for instance
 *  note that even if only "eax" and "edx" are of interest, other registers
 *  will be modified by the operation, so we need to tell the compiler about it.
 */
static inline void cpuid(int code, uint32_t *a, uint32_t *d) {
    asm volatile("cpuid":"=a"(*a),"=d"(*d):"a"(code):"ecx","ebx");
}
    
/** issue a complete request, storing general registers output as a string
 */
static inline int cpuid_string(int code, uint32_t where[4]) {
    asm volatile("cpuid":"=a"(*where),"=b"(*(where+1)),
                "=c"(*(where+2)),"=d"(*(where+3)):"a"(code));
    return (int)where[0];
}

// /* Example: Get CPU's model number */
// static int get_model(void)
// {
//     int ebx, unused;
//     cpuid(0, unused, ebx, unused, unused);
//     return ebx;
// }

/* Example: Check for builtin local APIC. */
int cpuid_has_mmx(void)
{
    unsigned int eax, edx;
    cpuid(1, &eax, &edx);
    return edx & CPUID_FEAT_EDX_MMX;
}

int cpuid_has_sse(void)
{
    unsigned int eax, edx;
    cpuid(1, &eax, &edx);
    return edx & CPUID_FEAT_EDX_SSE;
}

enum cpuid_requests {
    CPUID_GETVENDORSTRING,
    CPUID_GETFEATURES,
    CPUID_GETTLB,
    CPUID_GETSERIAL,
    
    CPUID_INTELEXTENDED=0x80000000,
    CPUID_INTELFEATURES,
    CPUID_INTELBRANDSTRING,
    CPUID_INTELBRANDSTRINGMORE,
    CPUID_INTELBRANDSTRINGEND,
};