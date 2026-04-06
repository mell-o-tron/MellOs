#include "drivers/network/ipv4.h"
#include "utils/format.h"
#include "data_structures/circular_list.h"

static CircularList* packets = NULL;

void ipv4_queue_packet(uint8_t* data){
    clist_append(&packets, data);
}

void ipv4_handle_packet() {
    if (packets == NULL) return;

    printf("[IPv4] packet received!\n");
    clist_remove(&packets, packets->data);
}