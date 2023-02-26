#ifndef ACPI_H
#define ACPI_H

#include <crt/header.h>
#include <stdint.h>

/* ACPI Header signatures */
#define ACPI_HEADER_RSDP "RSD PTR "
#define ACPI_HEADER_RSDT "RSDT"
/* Fixed tables */
#define ACPI_HEADER_MADT "APIC"
#define ACPI_HEADER_FADT "FACP"
/* AML tables */
#define ACPI_HEADER_DSDT "DSDT"
#define ACPI_HEADER_SSDT "SSDT"

EXTERN_C_BEGIN

/* Data from fixed ACPI tables (no AML) */
struct acpi_apic {
	uint32_t  ptr;     /* Local APIC physical address */
	uint8_t   cpu_id;  /* CPU ACPI ID */
	uint8_t   apic_id; /* APIC ID */
	uint32_t  LINT0;
	uint32_t  LINT1;
	uint32_t  num_nmis;
	uint32_t* nmi;
};

struct acpi_ioapic {
	uint32_t  ptr; /* IO APIC physical address */
	uint32_t  irq_min; /* Lowest IRQ handled */
	uint8_t   ioapic_id; /* IO APIC ID */
	uint32_t  num_isos; /* Interrupt source override */
	uint32_t* iso;
};

enum acpi_version {
	ACPI_VERSION_1 = 0,
	ACPI_VERSION_2 = 2,
};

struct acpi_fixed {
	uint32_t acpi_version;
	struct acpi_apic apic;
	struct acpi_ioapic ioapic;
	uint32_t acpi_table_num; /* Number of tables in the RSDT */
	uint32_t * acpi_tables; /* RSDT table of tables */
};

int acpi_init_fixed(struct acpi_fixed * data);

EXTERN_C_END

#endif
