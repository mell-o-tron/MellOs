#pragma once
#include "utils/typedefs.h"

void RTL_readMAC(uint8_t mac[6]);
void init_rtl8139 ();

#define RTL_RX 0x30     // RX Buffer Start
#define RTL_CR 0x37     // Command Register
#define RTL_CBR 0x3A    // Current Buffer Address
#define RTL_IMR 0x3C    // Interrupt Mask Register
#define RTL_ISR 0x3e    // Interrupt Status Register
#define RTL_RCR 0x44    // Receive Configuration Register
#define RTL_CAPR 0x38   // Current Address of Packet Read 


/* RCR: Receive Configuration Register */
#define RCR_ACCEPT_PHYS     (0b1 << 0)  // Accept all packets with physical destination address? (Promiscuous mode)
#define RCR_ACCEPT_MATCH    (0b1 << 1)  // Accept physical match packets?
#define RCR_ACCEPT_MC       (0b1 << 2)  // Accept physical multicast packets?
#define RCR_ACCEPT_BC       (0b1 << 3)  // Accept broadcast packets?
#define RCR_ACCEPT_RUNT     (0b1 << 4)  // Accept runt packets?
#define RCR_ACCEPT_ERR      (0b1 << 5)  // Accept error packets?
#define RCR_EEPROM_SEL      (0b1 << 6)  // 0: EEPROM used is 9346; 1: 9356. 
#define RCR_WRAP            (0b1 << 7)  // WRAP (if 1 rx must have 1.5k bytes after end. Invalid if len rx = 64k)

/* maximum size of the receive DMA data bursts */
typedef enum {
    RCR_MAX_DMA_64  = 0b010 << 8,
    RCR_MAX_DMA_128 = 0b011 << 8,
    RCR_MAX_DMA_256 = 0b100 << 8
} RCR_MAX_DMA;

/* rx ring buffer length in kb (16 bytes are added) */
typedef enum {
    RCR_RX_8           = 0b00 << 11,
    RCR_RX_16          = 0b01 << 11,
    RCR_RX_32          = 0b10 << 11,
    RCR_RX_64          = 0b11 << 11,
} RCR_BUF_LEN;

/* rx FIFO threshold: how many bytes in RTL FIFO before move to rx */
typedef enum {
    RCR_FIFO_TR_16     = 0b000 << 13,
    RCR_FIFO_TR_32     = 0b001 << 13,
    RCR_FIFO_TR_64     = 0b010 << 13,
    RCR_FIFO_TR_128    = 0b011 << 13,
    RCR_FIFO_TR_256    = 0b100 << 13,
    RCR_FIFO_TR_512    = 0b101 << 13,
    RCR_FIFO_TR_1024   = 0b110 << 13,
    RCR_FIFO_TR_NONE   = 0b111 << 13    // begins transfer after having recvd whole packet
} RCR_FIFO_THRESHOLD;

#define RCR_RCV_ER_8        (0b1 << 16)     // receive error packets larger than 8 bytes
#define RCR_MUL_ERINT       (0b1 << 17)     // multiple early interrupt selecgt

/* what fraction of packet transferred to buffer in early mode */
#define RCR_EARLY_RX_TR_NONE    (0b0000 << 24)
// TODO add other fractions...

/* IMR: interrupt mask register */
#define IMR_ROK (0b1)       // enable interrupt on receive
#define IMR_RER (0b1 << 1)  // enable interrupt on recv error
#define IMR_TOK (0b1 << 2)  // enable interrupt on transmit ok

/* CR: command register */
#define CR_BUFE (0b1)       // buffer empty (read only)
#define CR_TE   (0b1 << 2)  // transmit enable
#define CR_RE   (0b1 << 3)  // receive enable
#define CR_RST  (0b1 << 4)  // software reset, cleared on completion
