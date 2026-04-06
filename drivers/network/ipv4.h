#pragma once
#include "utils/typedefs.h"

#define IPv4_ADDR(z,y,x,w) (w << 24 | x << 16 | y << 8 | z)

extern uint32_t IPv4_HOST_ADDRESS;

// typedef struct IPv4_Packet {
//     uint8_t  version      :  4;
//     uint8_t  header_len   :  4;
//     uint8_t  dscp         :  6;
//     uint8_t  ecn          :  2;
//     uint16_t total_len    : 16;
//     uint16_t ident        : 16;
//     union {
//         uint8_t val       :  3;
//         struct {
//             bool reserved :  1;
//             bool no_frag  :  1;
//             bool more_frag:  1;
//         };
//     };
//     uint16_t frag_offset  : 13;
//     uint8_t  ttl          :  8;
//     uint8_t  protocol     :  8;
//     uint16_t checksum     : 16;
//     uint32_t src_addr     : 32;
//     uint32_t dst_addr     : 32;
//     uint8_t  data           [];
// } __attribute__((packed)) IPv4_Packet;

#define IPv4_GET_VER(x) (x[0] >> 4)
#define IPv4_GET_IHL(x) ((x[0] & 0x0F) * 4)
#define IPv4_GET_DSCP(x) (x[1] >> 2)
#define IPv4_GET_ECN(x) (x[1] & 0x03)
#define IPv4_GET_LEN(x) ((x[2] << 8) | x[3])
#define IPv4_GET_IDENT(x) ((x[4] << 8) | x[5])
#define IPv4_GET_ZERO(x) (x[6] >> 7)
#define IPv4_GET_NOFRAG(x) ((x[6] >> 6) & 1)
#define IPv4_GET_MOREFRAG(x) ((x[6] >> 5) & 1)
#define IPv4_GET_FRAG_OFFSET(x) (((x[6] & 0x1F) << 8) | x[7])
#define IPv4_GET_TTL(x) (x[8])
#define IPv4_GET_PROTO(x) (x[9])
#define IPv4_GET_CHECKSUM(x) ((x[10] << 8) | x[11])
#define IPv4_GET_SRC_ADDR(x) (x[15] << 24 | x[14] << 16 | x[13] << 8 | x[12])
#define IPv4_GET_DST_ADDR(x) (x[19] << 24 | x[18] << 16 | x[17] << 8 | x[16])
#define IPv4_GET_DATA(x) (x + IPv4_GET_IHL(x))
#define IPv4_GET_PAYLOAD_LEN(x) (IPv4_GET_LEN(x) - IPv4_GET_IHL(x))

typedef enum IPv4_PROTO {
    IPv4_PROTO_ICMP  =   1,
    IPv4_PROTO_IGMP  =   2,
    IPv4_PROTO_TCP   =   6,
    IPv4_PROTO_UDP   =  17,
    IPv4_PROTO_ENCAP =  41,
    IPv4_PROTO_OSPF  =  89,
    IPv4_PROTO_SCTP  = 132
} IPv4_PROTO;

void ipv4_queue_packet(uint8_t* data);
void ipv4_handle_packet();