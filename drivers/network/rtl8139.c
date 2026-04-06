#include "drivers/pci.h"
#include "drivers/port_io.h"
#include "rtl8139.h"
#include "memory/dynamic_mem.h"
#include "memory/mem.h"
#include "memory/paging/paging.h"
#include "cpu/interrupts/irq.h"
#include "utils/format.h"
#include "utils/conversions.h"
#ifdef VGA_VESA
#include "drivers/vesa/vesa_text.h"
#else
#include "drivers/vga_text.h"
#endif


static uint32_t RTL_IO_BASE = 0;

/* Linker-provided symbols */
extern char _rt_ring_start[];
extern char _rt_ring_end[];

__attribute__((section(".rt_tx_bufs"), aligned(32))) static uint8_t rtl_tx_bufs[4][16384];
__attribute__((section(".rt_ring"), aligned(32))) static uint8_t rtl_rx_buf[8192 + 16 + 1500];

void get_RTL_IO_BASE () {
    uint32_t BAR0 = getBAR (0, 3, 0); // for now forced value, this shall be changed
    RTL_IO_BASE = BAR0 & 0xFFFFFFFC;
}

void RTL_readMAC(uint8_t mac[6]) {
    if (RTL_IO_BASE == 0) {
        get_RTL_IO_BASE();
    }

    if (RTL_IO_BASE == 0) {
        kprint("RTL IO base was not set!\n");
        return;
    }
    
    for (int i = 0; i < 6; ++i) {
        mac[i] = inb(RTL_IO_BASE + i);
    }
}

#define RX_BUF_SIZE (8192 + 16)

void receive_packet () {
    uint16_t capr = inw(RTL_IO_BASE + RTL_CAPR);
    uint8_t* rx = rtl_rx_buf;

    while (1) {
        uint16_t cur = capr + 16;
        uint16_t cbr = inw(RTL_IO_BASE + RTL_CBR);

        if (cur == cbr)
            break;

        uint8_t* rx = rtl_rx_buf;
        uint16_t *header = (uint16_t *)(rx + cur);
        uint16_t status = header[0];
        uint16_t length = header[1];

        uint8_t rok = (status & 0b1) != 0;
        uint8_t fae = (status & (0b1 << 1)) != 0;
        uint8_t crc = (status & (0b1 << 2)) != 0;
        uint8_t lng = (status & (0b1 << 3)) != 0;
        uint8_t rnt = (status & (0b1 << 4)) != 0;
        uint8_t ise = (status & (0b1 << 5)) != 0;
        uint8_t bar = (status & (0b1 << 6)) != 0;
        uint8_t pam = (status & (0b1 << 7)) != 0;
        uint8_t mar = (status & (0b1 << 8)) != 0;
        
        // printf("cur=%x len=%x status=%x\n", cur, length, status);

        if (!(status & 0x01)) {
            kprint("Bad packet\n");

            printf("Receive ok: %x\n", rok);
            printf("Frame alignment error: %x\n", fae);
            printf("CRC error: %x\n", crc);
            printf("Long packet: %x\n", lng);
            printf("Runt packet: %x\n", rnt);
            printf("Invalid symbol error: %x\n", ise);
            printf("Broadcast addr recvd: %x\n", bar);
            printf("Physical addr recvd: %x\n", pam);
            printf("Multicast addr recvd: %x\n", mar);
        } else {
            uint8_t* data = (uint8_t*)(header + 2);
            
            EthernetFrame* frame = eth_populate_frame(data, length);
    
            printf("\n[RTL] Packet received! Payload: %s\n", frame -> payload);
    
            for (int i = 0; i < length / 2 + 2; i++){
                printf("%04X  ", ntohs(header[i]));
            }
            kprint("\n");

            eth_handle_frame(frame);
        }

        // advance
        capr = ((capr + length + 4 + 3) & ~3) % (RX_BUF_SIZE - 16);
        outw(RTL_IO_BASE + RTL_CAPR, capr);

        break;
    }
}

void rtl8139_handler(regs* r) {
    if (RTL_IO_BASE == 0) {
        kprint("RTL IO base was not set!\n");
        return;
    }

	uint16_t status = inw(RTL_IO_BASE + RTL_ISR);
	outw(RTL_IO_BASE + RTL_ISR, status);

	if(status & RTL_ISR_ROK) {
        // kprint("recvd\n");
        uint16_t phy_status = inw(RTL_IO_BASE + 0x6C);
        // printf("PHY Status: %04x\n", phy_status);

        // // Bit 2 = link up
        // if (phy_status & 0x04) {
        //     kprint("Link is UP\n");
        // } else {
        //     kprint("Link is DOWN - no network connection\n");
        // }

        uint16_t CAPR = inw(RTL_IO_BASE + RTL_CAPR);
        uint16_t CBPR = inw(RTL_IO_BASE + RTL_CBR);
        // printf("CAPR is: %x\n", CAPR);
        // printf("CBPR is: %x\n", CBPR);

        receive_packet();
	}
	if (status & RTL_ISR_TOK) {
        kprint("[RTL] Packet sent!\n");
	}
}

extern uint32_t page_directory[1024];

void init_tx_buffers() {
    for(int i = 0; i < RTL_TX_BUFNUM; i++) {
        uintptr_t bufaddr = get_physaddr(rtl_tx_bufs[i], page_directory);
        printf("[RTL] Init transmit buf %d with addr(virt/phys) 0x%X/0x%X\n", i, rtl_tx_bufs[i], bufaddr);
        outl(RTL_IO_BASE + RTL_TX_S(i), bufaddr);
        // printf("0x%X\n", RTL_TX_S(i));
        // printf("0x%X\n", RTL_TCR(i));
    }
}

void init_rtl8139 () {
    pci_enable_busmaster(0, 3, 0);
    get_RTL_IO_BASE();

    if (RTL_IO_BASE == 0) {
        kprint("RTL IO base was not set!\n");
        return;
    }
   
    // Set LWAKE + LWPTN
    outb(RTL_IO_BASE + 0x52, 0x0);
    // SW RESET!!! (note that the reset code used here (from specs) is != the one in osdev, idky)
    outb( RTL_IO_BASE + RTL_CR, CR_RST);

    // Wait for reset bit to clear
    while( (inb(RTL_IO_BASE + RTL_CR) & CR_RST) != 0) { }
    
    // Identity mapped, so not reeeally needed
    uintptr_t phys = get_physaddr(rtl_rx_buf, page_directory);
    
    // printf("RX: virt %x, phys %x\n", rtl_rx_buf, phys);

    // setup rx buffer
    memset(rtl_rx_buf, 0, 8192 + 16 + 1500);
    outl(RTL_IO_BASE + RTL_RX, (uint32_t)phys);

    // set interrupt mask register
    outw(RTL_IO_BASE + RTL_IMR, IMR_ROK | IMR_TOK);

    // Configuring receive buffer (RCR)
    outl(RTL_IO_BASE + RTL_RCR, 
          RCR_WRAP 
        | RCR_RX_8 
        | RCR_FIFO_TR_16 
        | RCR_ACCEPT_PHYS
        | RCR_ACCEPT_MATCH
        | RCR_ACCEPT_MC 
        | RCR_ACCEPT_BC
    );

    // initialize CAPR
    outw(RTL_IO_BASE + RTL_CAPR, 0xFFF0);

    // Enable Receive and Transmit
    outb(RTL_IO_BASE + RTL_CR, CR_TE | CR_RE);

    uint8_t irq_num = PCI_GetInterruptLine(0, 3);
    irq_install_handler(irq_num, rtl8139_handler);

    init_tx_buffers();
}

void rtl_transmit_data(void* data, size_t len) {
    static uint8_t tx_round_robin_index = 0;

    memcp(data, rtl_tx_bufs[tx_round_robin_index], len);

    RTL_TCR tcr;
    tcr.size = len;
    tcr.own_bit = 0;
    tcr.threshold = 1;

    printf("[RTL] Transmitting: ");
    for(int i = 0; i < len; i++) {
        printf("%02X ", rtl_tx_bufs[tx_round_robin_index][i]);
    }
    printf("\nTCR: 0x%04X\n", tcr.value);

    outl(RTL_IO_BASE + RTL_TCR(tx_round_robin_index), tcr.value);

    tx_round_robin_index = (tx_round_robin_index + 1) % RTL_TX_BUFNUM;
}

void rtl_transmit_eth_frame(EthernetFrame* frame) {
    static uint8_t tx_round_robin_index = 0;

    eth_unpack_frame(frame, rtl_tx_bufs[tx_round_robin_index]);

    RTL_TCR tcr;
    tcr.size = ETH_PAYLOAD_TO_FRAME_SIZE(frame->length);
    tcr.own_bit = 0;
    tcr.threshold = 1;

    printf("[RTL] Transmitting: ");
    for(int i = 0; i < tcr.size; i++) {
        printf("%02X ", rtl_tx_bufs[tx_round_robin_index][i]);
    }
    printf("\nTCR: 0x%04X\n", tcr.value);

    outl(RTL_IO_BASE + RTL_TCR(tx_round_robin_index), tcr.value);

    tx_round_robin_index = (tx_round_robin_index + 1) % RTL_TX_BUFNUM;
}