#ifndef E820_H
#define E820_H
#include <stdint.h>

typedef struct{
	uint32_t base;
	uint32_t length;
	uint16_t type;
	uint16_t acpi;
} SMAP32_t;

#endif
