#ifndef E820_H
#define E820_H
#include <stdint.h>
typedef struct{
	uint64_t base;
	uint64_t length;
	uint32_t type;
	uint32_t acpi;
} SMAP_entry_t;

typedef struct{
	uint32_t base;
	uint32_t length;
	uint16_t type;
	uint16_t acpi;
} SMAP32_t;

typedef struct{
	SMAP32_t* data;
	uint16_t n;
} return_t;

return_t validate(SMAP_entry_t* data, uint16_t n);
#endif
