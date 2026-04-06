#pragma once
#include "utils/typedefs.h"

typedef struct ARP_Packet {
    uint16_t hw_type;
    uint16_t proto_type;
    uint8_t  hw_addr_len;
    uint8_t  proto_addr_len;
    uint16_t operation;
    uint8_t  sender_hw_addr[6];
    uint32_t sender_proto_addr;
    uint8_t  recv_hw_addr[6];
    uint32_t recv_proto_addr;
} __attribute__((packed)) ARP_Packet;

#define ARP_HTYPE_ETHERNET 1
#define ARP_PROTO_IPv4     0x0800

void arp_queue_packet(uint8_t* data);
void arp_handle_packet();