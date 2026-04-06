#include "ethernet.h"
#include "memory/dynamic_mem.h"
#include "memory/mem.h"
#include "drivers/network/ipv4.h"
#include "drivers/network/arp.h"
#include "drivers/network/rtl8139.h"
#include "utils/format.h"
#include "utils/conversions.h"

static uint8_t mac_addr[6];

void eth_init() {
    RTL_readMAC(mac_addr);
}

EthernetFrame* eth_populate_frame(uint8_t* data, uint16_t length) {
    EthernetFrame* frame = kmalloc(sizeof(EthernetFrame));
    memcp(data, &frame->mac_dest, 6);
    memcp(data + 6, &frame->mac_src, 6);
    memcp(data + 12, &frame->ethertype_or_len, 2);
    frame->ethertype_or_len = ntohs(frame->ethertype_or_len);
    
    uint32_t len = ETH_FRAME_TO_PAYLOAD_SIZE(length);
    frame->length = len;

    frame->payload = kmalloc(len);
    memcp(data + 14, frame->payload, len);

    memcp(data + 14 + len, &frame->CRC, 4);

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

void eth_unpack_frame(EthernetFrame* frame, uint8_t* buffer) {
    memcp(&frame->mac_dest, buffer, 6);
    memcp(&frame->mac_src, buffer + 6, 6);
    memcp(&frame->ethertype_or_len, buffer + 12, 2);
    memcp(frame->payload, buffer + 14, frame->length);
    memcp(frame->CRC, buffer + 14 + frame->length, 4);
}

void eth_send_frame(uint8_t dst_mac[6], EtherType type, uint8_t* payload, uint16_t payload_size) {
    EthernetFrame frame;
    
    memcp(mac_addr, frame.mac_src, 6);
    memcp(dst_mac, frame.mac_dest, 6);
    
    frame.ethertype_or_len = htons(type);
    frame.payload = payload;
    frame.length = payload_size;

    rtl_transmit_eth_frame(&frame);
}