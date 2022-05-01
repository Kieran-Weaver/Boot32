#ifndef GDT_H
#define GDT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GDT_CS0 0x08
#define GDT_DS0 0x10
#define GDT_CS3 0x18
#define GDT_DS3 0x20
#define GDT_TSS 0x28

#pragma pack(push, 1)
struct GDTEntry {
	uint16_t low_limit;
	uint16_t low_base;

	uint8_t med_base;
	uint8_t access;
	uint8_t flags_limit;
	uint8_t high_base;
};

struct TSS {
	uint32_t reserved_0;
	
	uint32_t ESP0;
	uint16_t SS0;
	uint16_t reserved_1;
	
	uint32_t reserved_2[24];
};
#pragma pack(pop)


void gdt_set_tss(const struct TSS* tss);
void loadGDT(const struct GDTEntry* gdt, uint16_t size, uint16_t cs, uint16_t ds);
void setSegments(uint16_t cs, uint16_t ds);

#ifdef __cplusplus
}
#endif

#endif
