#ifndef ISA_DMA
#define ISA_DMA

#include "../Utils/Typedefs.h"

void maskChannel(uint8_t channel, int masked);
void initFloppyDMA(uint32_t address);
void prepare_for_floppyDMA_write();


#endif
