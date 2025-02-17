#include "gdt.h"

typedef struct
{
    uint16_t LimitLow;
    uint16_t BaseLow;
    uint8_t MiddleLow;
    uint8_t Access;
    uint8_t FlagLmitHi;
    uint8_t BaseHight;
} __attribute__((packed)) GDTEntry;

typedef struct
{
    uint16_t Limit;     // sizeof(gdt) - 1
    GDTEntry* Ptr;      // address of GDT
} __attribute__((packed)) GDTDescriptor; //How much does this sound stupid?

typedef enum
{
    // 'RW' - Code Segment 'Readable' -- Data Segment 'Writable'
    GDT_ACCESS_CODE_READABLE                = 0x02,
    GDT_ACCESS_DATA_WRITEABLE               = 0x02,

    // 'DC' - Data Segment 'Direction' -- Code Segment 'Conforming'
    GDT_ACCESS_CODE_CONFORMING              = 0x04,
    GDT_ACCESS_DATA_DIRECTION_NORMAL        = 0x00,
    GDT_ACCESS_DATA_DIRECTION_DOWN          = 0x04,

    // 'EX' + 'S' - Executable + Description type bit
    GDT_ACCESS_DATA_SEGMENT                 = 0x10,
    GDT_ACCESS_CODE_SEGMENT                 = 0x18,

    GDT_ACCESS_DESCRIPTOR_TSS               = 0x00,

    // 'DPL' - Descriptor Privilege Level (Ring)
    GDT_ACCESS_RING0                        = 0x00,
    GDT_ACCESS_RING1                        = 0x20,
    GDT_ACCESS_RING2                        = 0x40,
    GDT_ACCESS_RING3                        = 0x60,

    // 'P' - Present
    GDT_ACCESS_PRESENT                      = 0x80,

} GDT_ACCESS;

typedef enum 
{   
    //'L' + D/B - Long + Default Operation Size / Default Stack pointer Size / Upper Bound
    GDT_FLAG_64BIT                          = 0x20,
    GDT_FLAG_32BIT                          = 0x40,
    GDT_FLAG_16BIT                          = 0x00,

    // 'G' - Granularity
    GDT_FLAG_GRANULARITY_1B                 = 0x00,
    GDT_FLAG_GRANULARITY_4K                 = 0x80,
} GDT_FLAGS;


GDTEntry g_GDT[] = {
    // NULL descriptor
    ENTRY(0, 0, 0, 0),

    // Kernel 32-bit code segment
    ENTRY(
            0,
            0xFFFFF,
            GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE,
            GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K
        ),

    // Kernel 32-bit data segment
    ENTRY(    
            0,
            0xFFFFF,
            GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITEABLE,
            GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K
        ),

};

GDTDescriptor g_GDTDescriptor = { sizeof(g_GDT) - 1, g_GDT};

extern  void gdt_load(GDTDescriptor* descriptor, uint16_t codeSegment, uint16_t dataSegment);

/*And Finally... */
void gdt_init(){
    gdt_load(&g_GDTDescriptor, GDT_CODE_SEGMENT, GDT_DATA_SEGMENT);
}
