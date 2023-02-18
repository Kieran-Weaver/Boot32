#ifndef GDT_H
#define GDT_H

#include <stdint.h>
#include <crt/header.h>

EXTERN_C_BEGIN

#define GDT_CS0 0x08
#define GDT_DS0 0x10
#define GDT_CS3 0x18
#define GDT_DS3 0x20
#define GDT_TSS 0x28

#define MAX_IRQ 256
#define IRQ_INT  0x8E
#define IRQ_TRAP 0x8F
#define IRQ_TASK 0x85

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

struct IDTEntry {
	uint16_t isr_low;
	uint16_t kernel_cs;
	uint8_t  reserved;
	uint8_t  attribs;
	uint16_t isr_high;
};

struct DTR {
	uint16_t size;
	uint32_t offset;
};
#pragma pack(pop)


void GDT_init(const struct TSS* tss);
void loadGDT(const struct DTR* gdtr, uint16_t cs, uint16_t ds);

void IDT_init(void);
void IDT_setIRQ(uint8_t vector, const void* irq_addr, uint8_t flags);

EXTERN_C_END

#endif
