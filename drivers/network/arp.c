#include "drivers/network/arp.h"
#include "data_structures/circular_list.h"
#include "utils/format.h"
#include "utils/conversions.h"
#include "drivers/network/ethernet.h"
#include "drivers/network/rtl8139.h"
#include "drivers/network/ipv4.h"
#include "memory/mem.h"
#include "memory/dynamic_mem.h"

static CircularList* packets = NULL;

void arp_queue_packet(uint8_t* data){
    clist_append(&packets, data);
}

// Convert byte order for all fields of the packet
void arp_ntoh(ARP_Packet* packet) {
    packet->hw_type = ntohs(packet->hw_type);
    packet->proto_type = ntohs(packet->proto_type);
    packet->operation = ntohs(packet->operation);
}

void arp_reply(uint8_t sender_mac[6], uint32_t sender_proto);

void arp_handle_packet() {
    if (packets == NULL) return;

    // printf("ARP packet received!\n");

    ARP_Packet* packet = packets->data;
    arp_ntoh(packet);
    clist_remove(&packets, packets->data);

    printf("[ARP] Packet received:\nHTYPE: %04X, PTYPE: %04X, HLEN: %02d, PLEN: %02d, OPER: %s\nSHA: %02X:%02X:%02X:%02X:%02X:%02X, SPA: %03d.%03d.%03d.%03d\nTHA: %02X:%02X:%02X:%02X:%02X:%02X, TPA: %03d.%03d.%03d.%03d\n",
        packet->hw_type,
        packet->proto_type,
        packet->hw_addr_len,
        packet->proto_addr_len,
        packet->operation == 1 ? "REQ" : "REPL",
        packet->sender_hw_addr[0], packet->sender_hw_addr[1], packet->sender_hw_addr[2],
        packet->sender_hw_addr[3], packet->sender_hw_addr[4], packet->sender_hw_addr[5],
        ((uint8_t*)&packet->sender_proto_addr)[0],
        ((uint8_t*)&packet->sender_proto_addr)[1],
        ((uint8_t*)&packet->sender_proto_addr)[2],
        ((uint8_t*)&packet->sender_proto_addr)[3],
        packet->recv_hw_addr[0], packet->recv_hw_addr[1], packet->recv_hw_addr[2],
        packet->recv_hw_addr[3], packet->recv_hw_addr[4], packet->recv_hw_addr[5],
        ((uint8_t*)&packet->recv_proto_addr)[0],
        ((uint8_t*)&packet->recv_proto_addr)[1],
        ((uint8_t*)&packet->recv_proto_addr)[2],
        ((uint8_t*)&packet->recv_proto_addr)[3]
    );

    if(packet->recv_proto_addr == IPv4_HOST_ADDRESS){
        arp_reply(packet->sender_hw_addr, packet->sender_proto_addr);
    }

    kfree(packet);
}

void arp_reply(uint8_t sender_mac[6], uint32_t sender_proto) {
    ARP_Packet packet;
    packet.hw_type = htons(ARP_HTYPE_ETHERNET);
    packet.proto_type = htons(ARP_PROTO_IPv4);
    packet.hw_addr_len = 6;
    packet.proto_addr_len = 4;
    packet.operation = htons(ARP_OPERATION_REPLY);

    packet.sender_proto_addr = IPv4_HOST_ADDRESS;
    packet.recv_proto_addr = sender_proto;

    RTL_readMAC(packet.sender_hw_addr);
    memcp(sender_mac, packet.recv_hw_addr, 6);

    eth_send_frame(sender_mac, ETH_ARP, &packet, sizeof(ARP_Packet));
}