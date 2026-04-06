#pragma once
#include "utils/typedefs.h"
#include "drivers/network/ethernet.h"

void RTL_readMAC(uint8_t mac[6]);
void init_rtl8139 ();

#define RTL_TCR_0  0x10 // TX Command Register 0
#define RTL_TCR_1  0x14 // TX Command Register 1
#define RTL_TCR_2  0x18 // TX Command Register 2
#define RTL_TCR_3  0x1C // TX Command Register 3
#define RTL_TX_S_0 0x20 // TX Start Buffer 0
#define RTL_TX_S_1 0x24 // TX Start Buffer 1
#define RTL_TX_S_2 0x28 // TX Start Buffer 2
#define RTL_TX_S_3 0x2C // TX Start Buffer 3

#define RTL_TX_BUFNUM 4
#define RTL_TCR(x) (RTL_TCR_0 + x * sizeof(uint32_t))
#define RTL_TX_S(x) (RTL_TX_S_0 + x * sizeof(uint32_t))

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

#define RTL_ISR_TOK 0x04
#define RTL_ISR_ROK 0x01

typedef union RTL_TCR {
    uint32_t value;
    struct {
        uint16_t size           : 13;
        bool  own_bit           :  1;
        bool      tun           :  1;
        bool      tok           :  1;
        uint8_t threshold       :  6;
        uint8_t resv            :  2;
        uint8_t collision_count :  4;
        bool heart_beat         :  1;
        bool out_of_window_coll :  1;
        bool transmit_abort     :  1;
        bool carrier_sense_lost :  1;
    };
} RTL_TCR;

void rtl_transmit_data(void* data, size_t len);
void rtl_transmit_eth_frame(EthernetFrame* frame);
