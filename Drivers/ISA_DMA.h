#ifndef ISA_DMA
#define ISA_DMA

#include "../Utils/Typedefs.h"

void initFloppyDMA(uint32_t address);
void prepare_for_floppyDMA_write();

#endif
