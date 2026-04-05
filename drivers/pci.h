#pragma once // shame on you
#include "../utils/typedefs.h"

uint16_t PCI_GetVendor(uint8_t bus, uint8_t slot);
uint16_t PCI_GetDevice(uint8_t bus, uint8_t slot);
uint16_t PCI_GetStatus(uint8_t bus, uint8_t slot);
uint8_t PCI_GetInterruptLine(uint8_t bus, uint8_t slot);

uint32_t getBAR (uint8_t bus, uint8_t slot, uint8_t n);

void checkAllBuses();
void checkBARs(uint8_t bus, uint8_t slot);

void pci_enable_busmaster(uint8_t bus, uint8_t slot, uint8_t func);