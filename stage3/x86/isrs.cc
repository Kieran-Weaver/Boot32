#include <x86/gdt.h>
#include <x86/serial.h>
#include <x86/pic.h>
#include <crt/header.h>

EXTERN_C_BEGIN

__attribute__((interrupt)) void timer_isr(struct interrupt_frame* frame) {
	const char msg[] = "PIT interrupt received\r\n";

	ser_write(COM1, msg, sizeof(msg) - 1);

	pic_eoi(PIC_PIT);
}

__attribute__((interrupt)) void com1_isr(struct interrupt_frame* frame) {
	ser_flush(COM1);
	pic_eoi(PIC_COM1);
}

__attribute__((interrupt)) void spurious_irq7_isr(struct interrupt_frame* frame) {
	if (pic_read(PIC_ISR) & 0x80) pic_eoi(PIC_LPT1);
}

__attribute__((interrupt)) void spurious_irq15_isr(struct interrupt_frame* frame) {
	if (pic_read(PIC_ISR) & 0x8000) pic_eoi(PIC_LPT1);
	pic_eoi(PIC_CASCADE);
}

EXTERN_C_END
