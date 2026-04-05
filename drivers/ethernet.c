#include "ethernet.h"
#include "../memory/dynamic_mem.h"
#include "../memory/mem.h"

EthernetFrame* populate(uint8_t* data, uint16_t length) {
    EthernetFrame* frame = kmalloc(sizeof(EthernetFrame));
    memcp(data, frame->mac_dest, 6);
    memcp(data + 6, frame->mac_src, 6);
    frame->ethertype_or_len = data[12];
    
    uint32_t len = length - sizeof(EthernetFrame) + sizeof(uintptr_t);

    frame->payload = kmalloc(len);
    memcp(data + 14, frame->payload, len);

    frame->CRC = data[14 + sizeof(uintptr_t)];

    return frame;
}