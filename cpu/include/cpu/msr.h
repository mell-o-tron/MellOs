#pragma once
#include "stdint.h"

void cpu_get_MSR(uint32_t msr, uint32_t *lo, uint32_t *hi);
void cpu_set_MSR(uint32_t msr, uint32_t lo, uint32_t hi);