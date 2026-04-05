#include "pci.h"
#include "port_io.h"
#include "rtl8139.h"
#include "../memory/dynamic_mem.h"
#include "../memory/mem.h"
#include "../memory/paging/paging.h"
#include "../cpu/interrupts/irq.h"
#include "utils/format.h"
#ifdef VGA_VESA
#include "drivers/vesa/vesa_text.h"
#else
#include "drivers/vga_text.h"
#endif
#include "ethernet.h"


static uint32_t RTL_IO_BASE = 0;

/* Linker-provided symbols */
extern char _rt_ring_start[];
extern char _rt_ring_end[];

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
        uint16_t cur = (capr + 16) & (RX_BUF_SIZE - 1);
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
        
        printf("cur=%x len=%x status=%x\n", cur, length, status);

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
            break;
        }

        uint8_t* data = (uint8_t*)(header + 2);
        
        EthernetFrame* frame = populate(data, length);

        printf("Packet received! Payload: %s\n", frame -> payload);

        for (int i = 0; i < length / 2 + 2; i++){
            printf("%x  ", header[i]);
        }
        kprint("\n");

        // advance
        capr = capr + length + 2;
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

	if(status & 0x1) {
        kprint("recvd\n");
        uint16_t phy_status = inw(RTL_IO_BASE + 0x6C);
        printf("PHY Status: %04x\n", phy_status);

        // // Bit 2 = link up
        // if (phy_status & 0x04) {
        //     kprint("Link is UP\n");
        // } else {
        //     kprint("Link is DOWN - no network connection\n");
        // }

        uint16_t CAPR = inw(RTL_IO_BASE + 0x38);
        uint16_t CBPR = inw(RTL_IO_BASE + 0x3A);
        printf("CAPR is: %x\n", CAPR);
        printf("CBPR is: %x\n", CBPR);

        receive_packet();
	}
	if (status & 0x4) {
        kprint("sent\n");
	}
}

extern uint32_t page_directory[1024];

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
    
    printf("RX: virt %x, phys %x\n", rtl_rx_buf, phys);

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
        | RCR_ACCEPT_MC 
        | RCR_ACCEPT_BC
    );

    // initialize CAPR
    outw(RTL_IO_BASE + RTL_CAPR, 0xFFF0);

    // Enable Receive and Transmit
    outb(RTL_IO_BASE + RTL_CR, CR_TE | CR_RE);

    uint8_t irq_num = PCI_GetInterruptLine(0, 3);
    irq_install_handler(irq_num, rtl8139_handler);
}