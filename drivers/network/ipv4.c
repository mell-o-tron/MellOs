#include "drivers/network/ipv4.h"
#include "utils/format.h"
#include "data_structures/circular_list.h"
#include "memory/dynamic_mem.h"
#include "drivers/network/udp.h"

uint32_t IPv4_HOST_ADDRESS = IPv4_ADDR(192,168,100,2);

static CircularList* packets = NULL;

void ipv4_queue_packet(uint8_t* data){
    clist_append(&packets, data);
}

void ipv4_handle_packet() {
    if (packets == NULL) return;

    uint8_t* packet = packets->data;
    
    clist_remove(&packets, packets->data);
    
    printf("[IPv4] packet received!\n");
    uint32_t sender_addr = IPv4_GET_SRC_ADDR(packet);
    uint32_t destination_addr = IPv4_GET_DST_ADDR(packet);
    printf("VER: %d, IHL: %d, DSCP: %02X, ECN: %01X, LEN: %d, IDENT: %d, ZERO: %d\nNOFRAG: %d, MOREFRAG: %d, FRAG_OFFSET: %d, TTL: %d, PROTO: %02X\nSender: %03d.%03d.%03d.%03d, Destination: %03d.%03d.%03d.%03d\n",
        IPv4_GET_VER(packet),
        IPv4_GET_IHL(packet),
        IPv4_GET_DSCP(packet),
        IPv4_GET_ECN(packet),
        IPv4_GET_LEN(packet),
        IPv4_GET_IDENT(packet),
        IPv4_GET_ZERO(packet),
        IPv4_GET_NOFRAG(packet),
        IPv4_GET_MOREFRAG(packet),
        IPv4_GET_FRAG_OFFSET(packet), 
        IPv4_GET_TTL(packet),
        IPv4_GET_PROTO(packet),
        ((uint8_t*)&sender_addr)[0],
        ((uint8_t*)&sender_addr)[1],
        ((uint8_t*)&sender_addr)[2],
        ((uint8_t*)&sender_addr)[3],
        ((uint8_t*)&destination_addr)[0],
        ((uint8_t*)&destination_addr)[1],
        ((uint8_t*)&destination_addr)[2],
        ((uint8_t*)&destination_addr)[3]
    );

    for(int i = 0; i < IPv4_GET_PAYLOAD_LEN(packet); i++){
        printf("%02X ", IPv4_GET_DATA(packet)[i]);
    }
    printf("\n");

    switch(IPv4_GET_PROTO(packet)) {
        case IPv4_PROTO_ICMP: {
            printf("[IPv4] Proto: ICMP. Ignoring...\n");
            break;
        }
        case IPv4_PROTO_IGMP: {
            printf("[IPv4] Proto: IGMP. Ignoring...\n");
            break;
        }
        case IPv4_PROTO_TCP: {
            printf("[IPv4] Proto: TCP. Ignoring...\n");
            break;
        }
        case IPv4_PROTO_UDP: {
            printf("[IPv4] Passing to UDP layer\n");
            udp_handle_packet(IPv4_GET_DATA(packet), IPv4_GET_PAYLOAD_LEN(packet));
            break;
        }
        case IPv4_PROTO_ENCAP: {
            printf("[IPv4] Proto: ENCAP. Ignoring...\n");
            break;
        }
        case IPv4_PROTO_OSPF: {
            printf("[IPv4] Proto: OSPF. Ignoring...\n");
            break;
        }
        case IPv4_PROTO_SCTP: {
            printf("[IPv4] Proto: SCTP. Ignoring...\n");
            break;
        }
        default: {
            printf("[IPv4] Unknown protocol: %d. Ignoring...\n", IPv4_GET_PROTO(packet));
            break;
        }
    }

    kfree(packet);
}