#include <x86/serial.h>
#include <x86/intrinsics.h>
#include <crt/print.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

static const char assert_fmt[] = "Assertion failed: %s, (%s: %s: %u)";
static char tmp_buf[16];

static void assert_ser_write(char c) {
	/* Write single char to COM1 */
	while ((inb(SERIAL_COM1 + 5) & 0x20) != 0x20) {}
	outb(SERIAL_COM1, c);
}

void panic(const char* fmt, ...) {
	va_list args;
    va_start(args, fmt);
    
    cli();
    ser_init_poll();
    
    while (*fmt) {
		if (*fmt == '%') {
			const char* buf = getfmt(*(++fmt), tmp_buf, &args);
			while (*buf) {
				assert_ser_write(*(buf++));
			}
		} else {
			assert_ser_write(*fmt);
		}
        ++fmt;
    }
 
    va_end(args);
    
    while (1)
		hang();
}

void __assert_fail (const char * x, const char * file, int line, const char * func) {
	panic(assert_fmt, x, file, func, line);
}

#ifdef __cplusplus
}
#endif
