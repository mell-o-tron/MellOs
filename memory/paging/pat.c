#include "pat.h"
#include "cpu/msr.h"

// https://wiki.osdev.org/Paging#PAT

void setup_pat(){
    uint32_t lo, hi;
    cpu_get_MSR(PAT_MSR, &lo, &hi);
    hi = PAT_WRITE_COMBINING << 24 | PAT_WRITE_PROTECT << 16 | PAT_WRITE_COMBINING << 8 | PAT_WRITE_PROTECT;
    cpu_set_MSR(PAT_MSR, lo, hi);
}