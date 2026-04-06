#pragma once
#include "utils/typedefs.h"

void udp_handle_packet(uint8_t* data, uint16_t length);

#define UDP_GET_SRC_PORT(x) (x[0] << 8 | x[1])
#define UDP_GET_DST_PORT(x) (x[2] << 8 | x[3])
#define UDP_GET_LENGTH(x)   (x[4] << 8 | x[5])
#define UDP_GET_CHECKSUM(x) (x[6] << 8 | x[7])
#define UDP_GET_DATAGRAM(x) (x + 8)
