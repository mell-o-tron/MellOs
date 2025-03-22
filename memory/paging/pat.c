#include "pat.h"
#include "../cpu/msr.h"

void setup_pat(){
    uint32_t lo, hi;
    cpuGetMSR(PAT_MSR, &lo, &hi);
    hi = PAT_WRITE_COMBINING << 24 | PAT_WRITE_PROTECT << 16 | PAT_WRITE_COMBINING << 8 | PAT_WRITE_PROTECT;
    cpuSetMSR(PAT_MSR, lo, hi);
}