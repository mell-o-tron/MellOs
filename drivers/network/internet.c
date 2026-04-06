#include "drivers/network/internet.h"
#include "utils/typedefs.h"
#include "utils/format.h"
#include "drivers/network/ethernet.h"
#include "drivers/network/rtl8139.h"
#include "drivers/network/arp.h"
#include "drivers/network/ipv4.h"

void handle_packets_loop();

void network_task() {

    //checkAllBuses();
    //checkBARs(0,3);

    uint8_t mac [6];
    RTL_readMAC(mac);

    printf("%x:%x:%x:%x:%x:%x\n", mac [0], mac [1], mac [2], mac [3], mac [4], mac [5]);

    init_rtl8139 ();
    eth_init();

    handle_packets_loop();
}

void handle_packets_loop() {
    while(true) {
        asm volatile("" ::: "memory");
        arp_handle_packet();
        ipv4_handle_packet();
    }
}