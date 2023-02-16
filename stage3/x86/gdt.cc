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

#define ISR_NO_ERR_STUB(N, MSG) \
__attribute__((interrupt)) void isr_stub##N(struct interrupt_frame* frame) { \
	panic("Unexpected exception: " MSG); \
}

#define ISR_ERR_STUB(N, MSG) \
__attribute__((interrupt)) void isr_stub##N(struct interrupt_frame* frame, uint32_t error_code) { \
	panic("Unexpected exception: " MSG ", error code 0x%x", error_code); \
}

#define IDT_STUB(N) IDT_setIRQ(N, (void*)(isr_stub##N), IRQ_TRAP);
#define NMI_STUB(N) IDT_setIRQ(N, (void*)(isr_stub##N), IRQ_INT);

ISR_NO_ERR_STUB(0, "Division By 0")
ISR_NO_ERR_STUB(1, "Debug Trap")
ISR_NO_ERR_STUB(2, "NMI")
ISR_NO_ERR_STUB(3, "Breakpoint")
ISR_NO_ERR_STUB(4, "Overflow")
ISR_NO_ERR_STUB(5, "Bound Range Exceeded")
ISR_NO_ERR_STUB(6, "Invalid Opcode")
ISR_NO_ERR_STUB(7, "Device Not Available")
ISR_ERR_STUB(8, "Double Fault")
ISR_NO_ERR_STUB(9, "Coprocessor Segment Overrun")
ISR_ERR_STUB(10, "Invalid TSS")
ISR_ERR_STUB(11, "Segment Not Present")
ISR_ERR_STUB(12, "Stack Segment Fault")
ISR_ERR_STUB(13, "General Protection Fault")
ISR_ERR_STUB(14, "Page Fault")
ISR_NO_ERR_STUB(15, "Reserved 0xF")
ISR_NO_ERR_STUB(16, "x87 Exception")
ISR_ERR_STUB(17, "Alignment Check")
ISR_NO_ERR_STUB(18, "Machine Check")
ISR_NO_ERR_STUB(19, "SIMD Exception")
ISR_NO_ERR_STUB(20, "Virtualization Exception")
ISR_ERR_STUB(21, "Control Protection Exception")
ISR_NO_ERR_STUB(22, "Reserved 0x16")
ISR_NO_ERR_STUB(23, "Reserved 0x17")
ISR_NO_ERR_STUB(24, "Reserved 0x18")
ISR_NO_ERR_STUB(25, "Reserved 0x19")
ISR_NO_ERR_STUB(26, "Reserved 0x1A")
ISR_NO_ERR_STUB(27, "Reserved 0x1B")
ISR_NO_ERR_STUB(28, "Hypervisor Injection Exception")
ISR_ERR_STUB(29, "VMM Communication Exception")
ISR_ERR_STUB(30, "Security Exception")
ISR_NO_ERR_STUB(31, "Reserved 0x1F")

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
	IDT_STUB(0);
	IDT_STUB(1);
	NMI_STUB(2);
	IDT_STUB(3);
	IDT_STUB(4);
	IDT_STUB(5);
	IDT_STUB(6);
	IDT_STUB(7);
	IDT_STUB(8);
	IDT_STUB(9);
	IDT_STUB(10);
	IDT_STUB(11);
	IDT_STUB(12);
	IDT_STUB(13);
	IDT_STUB(14);
	IDT_STUB(15);
	IDT_STUB(16);
	IDT_STUB(17);
	IDT_STUB(18);
	IDT_STUB(19);
	IDT_STUB(20);
	IDT_STUB(21);
	IDT_STUB(22);
	IDT_STUB(23);
	IDT_STUB(24);
	IDT_STUB(25);
	IDT_STUB(26);
	IDT_STUB(27);
	IDT_STUB(28);
	IDT_STUB(29);
	IDT_STUB(30);
	IDT_STUB(31);
	
	idtr.offset = (uint32_t)(&IDT[0]);
	idtr.size   = (uint16_t)sizeof(struct IDTEntry) * MAX_IRQ - 1;

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
