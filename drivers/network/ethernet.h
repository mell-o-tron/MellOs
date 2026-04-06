#pragma once // shame on you
#include "utils/typedefs.h"

#define ETH_MAX_SIZE 0x0600

typedef enum EtherType {
    ETH_IPv4 = 0x0800,
    ETH_ARP  = 0x0806,
    ETH_IPv6 = 0x86DD
} EtherType;

#define ETH_FRAME_TO_PAYLOAD_SIZE(x) (x - 18) // total length - ethernet fields: SrcAddr(6) + DstAddr(6) + EtherType(2) + CRC(4)
#define ETH_PAYLOAD_TO_FRAME_SIZE(x) (x + 18)

typedef struct {
    uint8_t mac_dest [6];
    uint8_t mac_src [6];
    
    // there might be optional 802.1Q tag? idk
    
    uint16_t ethertype_or_len;
    uint16_t length;
    uint8_t* payload;
    uint32_t CRC;
} __attribute__((packed)) EthernetFrame;

void eth_init();

EthernetFrame* eth_populate_frame(uint8_t* data, uint16_t length);
void eth_handle_frame(EthernetFrame* frame);
void eth_unpack_frame(EthernetFrame* frame, uint8_t* buffer);
void eth_send_frame(uint8_t dst_mac[6], EtherType type, uint8_t* payload, uint16_t payload_size);