#include <x86/gdt.h>
#include <x86/serial.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((interrupt)) void timer_isr(struct interrupt_frame* frame) {
	const char *msg = "PIT interrupt received\r\n";
	struct serial port;

	port.port = COM1;
	port.speed = B115200;
	while (*msg) {
		ser_write(&port, *(msg++));
	}
}

#ifdef __cplusplus
}
#endif
