#pragma once // shame on you
#include "utils/typedefs.h"

#define ETH_MAX_SIZE 0x0600
#define ETH_IPv4     0x0800
#define ETH_ARP      0x0806
#define ETH_IPv6     0x86DD

typedef struct {
    uint8_t mac_dest [6];
    uint8_t mac_src [6];
    
    // there might be optional 802.1Q tag? idk
    
    uint16_t ethertype_or_len;
    uint8_t* payload;
    uint32_t CRC;
} __attribute__((packed)) EthernetFrame;

EthernetFrame* eth_populate_frame(uint8_t* data, uint16_t length);
void eth_handle_frame(EthernetFrame* frame);