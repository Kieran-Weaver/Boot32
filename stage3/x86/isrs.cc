#include <x86/gdt.h>
#include <x86/serial.h>
#include <x86/pic.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((interrupt)) void timer_isr(struct interrupt_frame* frame) {
	const char msg[] = "PIT interrupt received\r\n";

	ser_write(COM1, msg, sizeof(msg) - 1);

	pic_eoi(PIC_PIT);
}

__attribute__((interrupt)) void com1_isr(struct interrupt_frame* frame) {
	ser_flush(COM1);
	pic_eoi(PIC_COM1);
}

#ifdef __cplusplus
}
#endif
