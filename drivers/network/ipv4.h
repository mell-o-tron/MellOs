#pragma once
#include "utils/typedefs.h"

#define IPv4_ADDR(z,y,x,w) (w << 24 | x << 16 | y << 8 | z)

extern uint32_t IPv4_HOST_ADDRESS;

void ipv4_queue_packet(uint8_t* data);
void ipv4_handle_packet();