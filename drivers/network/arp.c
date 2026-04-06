#include "drivers/network/arp.h"
#include "data_structures/circular_list.h"
#include "utils/format.h"
#include "utils/conversions.h"

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
}