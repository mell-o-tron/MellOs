#pragma once // shame on you
#include "../utils/typedefs.h"

typedef struct {
    uint8_t mac_dest [6];
    uint8_t mac_src [6];
    
    // there might be optional 802.1Q tag? idk
    
    uint16_t ethertype_or_len;
    uint8_t* payload;
    uint32_t CRC;
} __attribute__((packed)) EthernetFrame;

EthernetFrame* populate(uint8_t* data, uint16_t length);