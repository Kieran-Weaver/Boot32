#include <x86/gdt.h>

extern struct GDTEntry GDT[];
extern struct GDTEntry GDT_TSSEntry;

void gdt_set_tss(const struct TSS* tss) {
	GDT_TSSEntry.low_limit = sizeof(struct TSS);
	GDT_TSSEntry.low_base  = ((uintptr_t)tss) & 0xFFFF;
	GDT_TSSEntry.med_base  = (((uintptr_t)tss) >> 16) & 0xFF;
	GDT_TSSEntry.high_base = ((uintptr_t)tss) >> 24;
	GDT_TSSEntry.access = 0x89;
	
	loadGDT(GDT, GDT_TSS + 0x8, GDT_CS0, GDT_DS0);
}
