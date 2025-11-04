#include "autoconf.h"

#include "cpu/gdt.h"
#include "stdint.h"
#include "memory_area_spec.h"
#include "vesa.h"

typedef struct {
	uint16_t LimitLow;
	uint16_t BaseLow;
	uint8_t MiddleLow;
	uint8_t Access;
	uint8_t FlagLmitHi;
	uint8_t BaseHight;
} __attribute__((packed)) GDTEntry;

typedef struct {
	uint16_t Limit;                      // sizeof(gdt) - 1
	GDTEntry* Ptr;                       // address of GDT
} __attribute__((packed)) GDTDescriptor; // How much does this sound stupid?

typedef enum {
	// 'RW' - Code Segment 'Readable' -- Data Segment 'Writable'
	GDT_ACCESS_CODE_READABLE = 0x02,
	GDT_ACCESS_DATA_WRITEABLE = 0x02,

	// 'DC' - Data Segment 'Direction' -- Code Segment 'Conforming'
	GDT_ACCESS_CODE_CONFORMING = 0x04,
	GDT_ACCESS_DATA_DIRECTION_NORMAL = 0x00,
	GDT_ACCESS_DATA_DIRECTION_DOWN = 0x04,

	// 'EX' + 'S' - Executable + Description type bit
	GDT_ACCESS_DATA_SEGMENT = 0x10,
	GDT_ACCESS_CODE_SEGMENT = 0x18,

	GDT_ACCESS_DESCRIPTOR_TSS = 0x00,

	// 'DPL' - Descriptor Privilege Level (Ring)
	GDT_ACCESS_RING0 = 0x00,
	GDT_ACCESS_RING1 = 0x20,
	GDT_ACCESS_RING2 = 0x40,
	GDT_ACCESS_RING3 = 0x60,

	// 'P' - Present
	GDT_ACCESS_PRESENT = 0x80,

} GDT_ACCESS;

typedef enum {
	//'L' + D/B - Long + Default Operation Size / Default Stack pointer Size / Upper Bound
	GDT_FLAG_64BIT = 0x20,
	GDT_FLAG_32BIT = 0x40,
	GDT_FLAG_16BIT = 0x00,

	// 'G' - Granularity
	GDT_FLAG_GRANULARITY_1B = 0x00,
	GDT_FLAG_GRANULARITY_4K = 0x80,
} GDT_FLAGS;

extern char __text_va_start[];
extern char __text_pa_end[];
extern char __text_pa_start[];
extern char __text_delta[];

extern char __data_va_start[];
extern char __data_pa_end[];
extern char __data_pa_start[];

extern char __rodata_va_start[];
extern char __rodata_pa_end[];
extern char __rodata_pa_start[];

extern char __bss_va_start[];
extern char __bss_pa_end[];
extern char __bss_pa_start[];

static GDTEntry make_gdt_entry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
	GDTEntry entry;

	entry.LimitLow = limit & 0xFFFF;
	entry.BaseLow = base & 0xFFFF;
	entry.MiddleLow = (base >> 16) & 0xFF;
	entry.Access = access;
	entry.FlagLmitHi = ((limit >> 16) & 0x0F) | (flags & 0xF0);
	entry.BaseHight = (base >> 24) & 0xFF;

	return entry;
}

GDTEntry g_GDT[11] = {0};

GDTDescriptor g_GDTDescriptor = {sizeof(g_GDT) - 1, g_GDT};

extern void gdt_load(uint16_t length, GDTEntry* entries, uint16_t codeSegment, uint16_t dataSegment);

/*And Finally... */
void gdt_init() {
	// this uses virtual addresses as we have already initialized paging

	g_GDT[0] = make_gdt_entry(0, 0, 0, 0);

	g_GDT[1] = make_gdt_entry(0, 0x7FF, GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_RING0 | GDT_ACCESS_PRESENT |
		GDT_ACCESS_CODE_READABLE,
		GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K);
	g_GDT[2] = make_gdt_entry(0, 0x7FF, GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_RING0 | GDT_ACCESS_PRESENT |
		GDT_ACCESS_DATA_WRITEABLE,
		GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K);

	// Kernel 32-bit code segment
	g_GDT[3] = make_gdt_entry((int)__text_va_start, (int)__text_pa_end - (int)__text_pa_start,
	                          GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_SEGMENT |
	                              GDT_ACCESS_CODE_READABLE,
	                          GDT_FLAG_32BIT);

	// Kernel 32-bit data segment
	g_GDT[4] = make_gdt_entry((int)__rodata_va_start, (int)__rodata_pa_end - (int)__rodata_pa_start,
	                          GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEGMENT,
	                          GDT_FLAG_32BIT);

	g_GDT[5] = make_gdt_entry((int)__data_va_start, (int)__data_pa_end - (int)__data_pa_start,
	                          GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEGMENT |
	                              GDT_ACCESS_DATA_WRITEABLE,
	                          GDT_FLAG_32BIT);

	g_GDT[6] = make_gdt_entry((int)__bss_va_start, (int)__bss_pa_end - (int)__bss_pa_start,
	                          GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEGMENT |
	                              GDT_ACCESS_DATA_WRITEABLE,
	                          GDT_FLAG_32BIT);
	g_GDT[7] = make_gdt_entry(UPPER_KERNEL_STACK_BASE, UPPER_KERNEL_STACK_TOP,
	                          GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEGMENT |
	                              GDT_ACCESS_DATA_WRITEABLE | GDT_ACCESS_DATA_DIRECTION_DOWN,
	                          GDT_FLAG_32BIT);
	// user heap (0x800000 - 0xBFFFFF, 4MB, r3)
	g_GDT[8] = make_gdt_entry(HEAP_START, HEAP_END - HEAP_START,
							  GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_DATA_SEGMENT |
								  GDT_ACCESS_DATA_WRITEABLE,
							  GDT_FLAG_32BIT);

	// kernel heap (0xC00000 - 0xFFFFFF, 4MB, r0)
	g_GDT[9] = make_gdt_entry(KERNEL_HEAP_START, KERNEL_HEAP_END - KERNEL_HEAP_START,
							   GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEGMENT |
								   GDT_ACCESS_DATA_WRITEABLE,
							   GDT_FLAG_32BIT);
	g_GDT[10] = make_gdt_entry(FRAMEBUFFER_VIRT_START,
#ifdef CONFIG_GFX_VESA
		FRAMEBUFFER_SIZE_BYTES(vga_fb->height, vga_fb->pitch) - 1,
#else
		64,
#endif
								GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEGMENT |
								GDT_ACCESS_DATA_WRITEABLE,
								GDT_FLAG_32BIT);
	gdt_load(sizeof(g_GDT)-1, g_GDT, GDT_CODE_SEGMENT, GDT_DATA_SEGMENT);
}
