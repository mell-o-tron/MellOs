#pragma once

#define PAT_MSR               0x277

typedef enum{
    PAT_UNCACHEABLE         = 0b0,
    PAT_WRITE_COMBINING     = 0b1,
    PAT_WRITE_THROUGH       = 0b100,
    PAT_WRITE_PROTECT       = 0b101,
    PAT_WRITE_BACK          = 0b110,
    PAT_UNCACHED            = 0b111
} PAT_ATTRIBUTE;

void setup_pat();