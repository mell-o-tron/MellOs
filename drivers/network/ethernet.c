#include "ethernet.h"
#include "memory/dynamic_mem.h"
#include "memory/mem.h"
#include "drivers/network/ipv4.h"
#include "drivers/network/arp.h"
#include "utils/format.h"
#include "utils/conversions.h"

EthernetFrame* eth_populate_frame(uint8_t* data, uint16_t length) {
    EthernetFrame* frame = kmalloc(sizeof(EthernetFrame));
    memcp(data, &frame->mac_dest, 6);
    memcp(data + 6, &frame->mac_src, 6);
    memcp(data + 12, &frame->ethertype_or_len, 2);
    frame->ethertype_or_len = ntohs(frame->ethertype_or_len);
    
    uint32_t len = length - sizeof(EthernetFrame) + sizeof(uintptr_t);

    frame->payload = kmalloc(len);
    memcp(data + 14, frame->payload, len);

    memcp(data + 14 + sizeof(uintptr_t), &frame->CRC, 4);

    return frame;
}

void eth_handle_frame(EthernetFrame* frame) {
    switch(frame->ethertype_or_len) {
        case ETH_IPv4: {
            printf("[ETH] Passing frame to IPv4 layer\n");
            ipv4_queue_packet(frame->payload);
            break;
        }
        case ETH_IPv6: {
            printf("[ETH] IPv6 layer packet got. Ignoring...\n");
            break;
        }
        case ETH_ARP: {
            printf("[ETH] Passing frame to ARP layer\n");
            arp_queue_packet(frame->payload);
            break;
        }
        default: {
            printf("[ETH] Unknown packet type. Ethertype is: %04X\n", frame->ethertype_or_len);
            for(int i = 0; i < 256; i++) {
                printf("%02X ", ((uint8_t*)frame)[i]);
            }
            break;
        }
    }
}