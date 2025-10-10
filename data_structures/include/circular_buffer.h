#pragma once
#include "stdint.h"
#include "stdbool.h"

typedef struct {
    char * array;
    uint32_t top;
    uint32_t bot;
    uint32_t size;
} cbuffer_t;


void add_to_cbuffer(cbuffer_t * buf, char c, bool is_uppercase);

char get_from_cbuffer(cbuffer_t * buf);

void rem_from_cbuffer(cbuffer_t * buf);
