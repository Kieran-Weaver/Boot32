#include <x86/acpi.h>
#include <mem/kmalloc.h>
#include <crt/util.h>
#include <crt/assert.h>

#define EBDA_START (void*)0x80000
#define EBDA_END   (void*)0xA0000

#define BIOS_START (void*)0xE0000
#define BIOS_END   (void*)0x100000

EXTERN_C_BEGIN

typedef const void* cv_p;

static const char* rsdp_sig = ACPI_HEADER_RSDP;

struct RSDP {
	char Signature[8];
	uint8_t Checksum;
	char OEMID[6];
	uint8_t Revision;
	uint32_t RsdtAddress;
} __attribute__ ((packed));

struct SDT {
	char Signature[4];
	uint32_t Length;
	uint8_t Revision;
	uint8_t Checksum;
	char OEMID[6];
	char OEMTableID[8];
	uint32_t OEMRevision;
	uint32_t CreatorID;
	uint32_t CreatorRevision;
} __attribute__ ((packed));

static cv_p find_rsdp(void) {
	for (cv_p i = EBDA_START; i < EBDA_END; i += 0x10) {
		if (memcmp(i, rsdp_sig, 8) == 0) {
			return i;
		}
	}

	for (cv_p i = BIOS_START; i < BIOS_END; i += 0x10) {
		if (memcmp(i, rsdp_sig, 8) == 0) {
			return i;
		}
	}

	return NULL;
}

static uint8_t checksum(cv_p block, uint32_t size) {
	uint8_t csum = 0;
	const uint8_t* data = block;

	for (uint32_t i = 0; i < size; i++) {
		csum += data[i];
	}

	return csum;
}

int acpi_init_fixed(struct acpi_fixed * data) {
	const struct RSDP* rsdp = find_rsdp();

	assert(rsdp);

	assert( rsdp->Revision == ACPI_VERSION_1 );

	assert(checksum( rsdp, sizeof(struct RSDP) ) == 0);

	const struct SDT* rsdt = (const struct SDT*)vmap( rsdp->RsdtAddress, PAGE_SIZE );

	return 1;
}

EXTERN_C_END
