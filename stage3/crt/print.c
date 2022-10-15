#include <crt/print.h>
#include <stddef.h>

static char* fmt_uint(char* buf, unsigned int arg, unsigned int base) {
	const char hex[] = "0123456789ABCDEF";
	
	buf += 12;
	*(--buf) = '\0';
	do {
		*(--buf) = hex[arg % base];
		arg /= 10;
	} while (arg);

	return buf;
}

const char* getfmt(char fmt, char* buf, va_list* vlist) {
	char* ret = buf;
	int signed_arg = 0;
	unsigned int unsigned_arg = 0;

	switch (fmt) {
		case '%':
			buf[0] = '%';
			buf[1] = '\0';
			break;
		case 'c':
			buf[0] = va_arg(*vlist, int);
			buf[1] = '\0';
			break;
		case 's':
			ret = va_arg(*vlist, char*);
			break;
		case 'i':
		case 'd':
			signed_arg = va_arg(*vlist, int);
			if (signed_arg < 0) {
				unsigned_arg = -signed_arg;
			} else {
				unsigned_arg = signed_arg;
			}
			ret = fmt_uint(buf + 1, unsigned_arg, 10);
			if (signed_arg < 0)
				*(--ret) = '-';
			break;
		case 'u':
			ret = fmt_uint(buf, va_arg(*vlist, unsigned int), 10);
			break;
		case 'x':
			ret = fmt_uint(buf, va_arg(*vlist, unsigned int), 16);
			break;
		default:
			ret = NULL;
			break;
	}
	
	return ret;
}
