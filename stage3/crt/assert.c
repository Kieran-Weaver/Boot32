#include <x86/serial.h>

#ifdef __cplusplus
extern "C" {
#endif

static const char assert_txt[] = "Assertion failed: ";
static struct serial port;
static char tmp_buf[13];

static void assert_print(const char* str) {
	port.port = COM1;
	port.speed = B115200;

	for (uint16_t i = 0; str[i]; i++) {
		ser_write(&port, str[i]);
	}
}

static char* assert_itoa(char* str, int x) {
	str += 12;
	*(--str) = '\0';
	do {
		*(--str) = '0' + (x % 10);
		x /= 10;
	} while (x);
	return str;
}

void __assert_fail (const char * x, const char * file, int line, const char * func) {
	assert_print(assert_txt);
	assert_print(x);
	assert_print(" (");
	assert_print(file);
	assert_print(": ");
	assert_print(func);
	assert_print(": ");
	assert_print(assert_itoa(tmp_buf, line));
	assert_print(") \r\n");
		
	while (1)
		asm("cli; hlt");
}

#ifdef __cplusplus
}
#endif
