#include "drivers/network/udp.h"
#include "data_structures/circular_list.h"
#include "utils/format.h"

void udp_handle_packet(uint8_t* data, uint16_t length) {
    printf("[UDP] Packet received!\n");

    printf("SRC_PORT: %05d, DST_PORT: %05d, LENGTH: %d, CHECKSUM: %04X\n",
        UDP_GET_SRC_PORT(data),
        UDP_GET_DST_PORT(data),
        UDP_GET_LENGTH(data),
        UDP_GET_CHECKSUM(data)
    );

    printf("Payload: %s\n", UDP_GET_DATAGRAM(data));
}