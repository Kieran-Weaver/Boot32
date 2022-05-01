#include <x86/gdt.h>
#include <crt/assert.h>
#include <x86/intrinsics.h>

/* For initialization reasons, GDT located in crt0.S */
extern struct GDTEntry GDT[];
extern struct GDTEntry GDT_TSSEntry;

static struct DTR gdtr alignas(4);
static struct DTR idtr alignas(4);

static struct IDTEntry alignas(16) IDT[MAX_IRQ];

struct interrupt_frame {
	uint32_t ip;
	uint32_t cs;
	uint32_t flags;
	uint32_t sp;
	uint32_t ss;
};

__attribute__((interrupt)) void isr_div0(struct interrupt_frame* frame) {
	assert(0);
}

__attribute__((interrupt)) void isr_no_err_stub(struct interrupt_frame* frame) {
	assert(0);
}

__attribute__((interrupt)) void isr_err_stub(struct interrupt_frame* frame, uint32_t error_code) {
	assert(0);
}

void GDT_init(const struct TSS* tss) {
	gdtr.offset = (uint32_t)(&GDT[0]);
	gdtr.size   = GDT_TSS + 8;
	
	GDT_TSSEntry.low_limit = sizeof(struct TSS);
	GDT_TSSEntry.low_base  = ((uintptr_t)tss) & 0xFFFF;
	GDT_TSSEntry.med_base  = (((uintptr_t)tss) >> 16) & 0xFF;
	GDT_TSSEntry.high_base = ((uintptr_t)tss) >> 24;
	GDT_TSSEntry.access = 0x89;
	
	loadGDT(&gdtr, GDT_CS0, GDT_DS0);
}

void IDT_init(void) {
	const uint8_t is_error[] = {
		0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 1, 1, 1, 1, 1, 0,
		0, 1, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 0
	};

	idtr.offset = (uint32_t)(&IDT[0]);
	idtr.size   = (uint16_t)sizeof(struct IDTEntry) * MAX_IRQ - 1;

	IDT_setIRQ(0, (void*)isr_div0, IRQ_INT);
	for (uint8_t vector = 1; vector < 32; vector++) {
		if (is_error[vector]) {
			IDT_setIRQ(vector, (void*)isr_err_stub, IRQ_INT);
		} else {
			IDT_setIRQ(vector, (void*)isr_no_err_stub, IRQ_INT);
		}
	}
	
	asm volatile ("lidt %0" : : "m"(idtr));
}

void IDT_setIRQ(uint8_t vector, const void* isr, uint8_t flags) {
	struct IDTEntry* desc = &IDT[vector];
	
	desc->isr_low   = (uint32_t)isr & 0xFFFF;
	desc->kernel_cs = GDT_CS0;
	desc->reserved  = 0;
	desc->attribs   = flags;
	desc->isr_high  = (uint32_t)isr >> 16;
}
