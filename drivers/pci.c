#include "pci.h"
#include "port_io.h"
#include "utils/format.h"
#ifdef VGA_VESA
#include "drivers/vesa/vesa_text.h"
#else
#include "drivers/vga_text.h"
#endif

uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
  
    // Create configuration address as per Figure 1 of https://wiki.osdev.org/PCI
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
  
    // Write out the address
    outl(0xCF8, address);
    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    tmp = (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}

uint32_t pciConfigReadDword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address = (uint32_t)(
        (1U << 31) |
        ((uint32_t)bus << 16) |
        ((uint32_t)slot << 11) |
        ((uint32_t)func << 8) |
        (offset & 0xFC)
    );
    outl(0xCF8, address);
    return inl(0xCFC);
}

void pciConfigWriteDword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value) {
    uint32_t address = (uint32_t)(
        (1U << 31) |
        ((uint32_t)bus << 16) |
        ((uint32_t)slot << 11) |
        ((uint32_t)func << 8) |
        (offset & 0xFC)
    );
    outl(0xCF8, address);
    outl(0xCFC, value);
}

void pciConfigWriteWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint16_t value) {
    uint8_t aligned_off = offset & 0xFC;
    uint32_t cur = pciConfigReadDword(bus, slot, func, aligned_off);
    uint8_t shift = (offset & 2) * 8;
    uint32_t mask = 0xFFFFu << shift;
    uint32_t newval = (cur & ~mask) | ((uint32_t)value << shift);
    pciConfigWriteDword(bus, slot, func, aligned_off, newval);
}

uint16_t PCI_GetVendor(uint8_t bus, uint8_t slot) {
    uint16_t vendor, device;
    /* Try and read the first configuration register. Since there are no
     * vendors that == 0xFFFF, it must be a non-existent device. */
    if ((vendor = pciConfigReadWord(bus, slot, 0, 0)) == 0xFFFF) {
       // device = pciConfigReadWord(bus, slot, 0, 2);
    //    kprint("UMMARONNA CHE È");
    ;
    }
    
    // on error returns 0xFFFF
    return vendor;
}

uint16_t PCI_GetDevice(uint8_t bus, uint8_t slot) {
    return pciConfigReadWord(bus, slot, 0, 2);
}

uint16_t PCI_GetStatus(uint8_t bus, uint8_t slot) {
    return pciConfigReadWord(bus, slot, 0, 6);
}

uint8_t PCI_GetHeaderType(uint8_t bus, uint8_t slot) {
    return (uint8_t) (pciConfigReadWord(bus, slot, 0, 14) & 0xF);
}


uint8_t PCI_GetInterruptLine(uint8_t bus, uint8_t slot) {
    return (uint8_t) (pciConfigReadWord(bus, slot, 0, 0x3C) & 0xF);
}

void checkDevice (uint8_t bus, uint8_t slot) {
    uint16_t vendor = PCI_GetVendor(bus, slot);
    uint16_t device = PCI_GetDevice(bus, slot);
    uint16_t status = PCI_GetStatus(bus, slot);
    uint16_t header_type = PCI_GetHeaderType(bus, slot);

    if (vendor != 0xFFFF) {
        printf("bus %x\n", bus);
        printf("slot %x\n", slot);
        printf("vendor %x\n", vendor);
        printf("device id %x\n", device);
        printf("device status %x\n", status);
        // printf("header type %x\n", header_type);
    } else {
        ;//printf("UMMARONNA CHE È %x:%x\n", bus, slot);
    }
}

uint32_t getBAR (uint8_t bus, uint8_t slot, uint8_t n) {
    uint8_t header_type = PCI_GetHeaderType(bus, slot);
    
    if (header_type == 0) {
        uint16_t low_part = pciConfigReadWord(bus, slot, 0, 0x10 + 4*n);
        uint16_t high_part = pciConfigReadWord(bus, slot, 0, 0x12 + 4*n);
        return low_part | ((uint32_t) high_part << 16);
    } else {
        kprint("Not a standard PCI device\n"); 
    }
    return 0;
}

void checkAllBuses() {
    uint16_t bus;
    uint8_t device;

    for (bus = 0; bus < 256; bus++) {
        for (device = 0; device < 32; device++) {
            checkDevice(bus, device);
        }
    }
}

void checkBARs(uint8_t bus, uint8_t slot) {
    printf("BAR0: %x \n", getBAR (bus, slot, 0));
    printf("BAR1: %x \n", getBAR (bus, slot, 1));
    printf("BAR2: %x \n", getBAR (bus, slot, 2));
    printf("BAR3: %x \n", getBAR (bus, slot, 3));
    printf("BAR4: %x \n", getBAR (bus, slot, 4));
    printf("BAR5: %x \n", getBAR (bus, slot, 5));
}

void pci_enable_busmaster(uint8_t bus, uint8_t slot, uint8_t func) {
    uint16_t cmd = pciConfigReadWord(bus, slot, func, 0x04);
    cmd |= 0x4; // set Bus Master (0x4)
    pciConfigWriteWord(bus, slot, func, 0x04, cmd);
}