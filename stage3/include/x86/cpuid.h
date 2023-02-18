#ifndef CPUID_H
#define CPUID_H

#include <crt/header.h>
#include <stdint.h>

EXTERN_C_BEGIN

enum cpuid_flags {
	CPUID_FPU = 0,
	CPUID_VME,
	CPUID_DE,
	CPUID_PSE,
	CPUID_TSC,
	CPUID_MSR,
	CPUID_PAE,
	CPUID_MCE,
	CPUID_CX8,
	CPUID_APIC,
	CPUID_RESERVED_1,
	CPUID_SEP,
	CPUID_MTRR,
	CPUID_PGE,
	CPUID_MCA,
	CPUID_CMOV,
	CPUID_PAT,
	CPUID_PSE36,
	CPUID_PSN,
	CPUID_CLFSH,
	CPUID_RESERVED_2,
	CPUID_DS,
	CPUID_ACPI,
	CPUID_MMX,
	CPUID_FXSR,
	CPUID_SSE,
	CPUID_SSE2,
	CPUID_SS,
	CPUID_HTT,
	CPUID_TM,
	CPUID_IA64,
	CPUID_PBE
};

void cpuid_init(void);
uint32_t cpuid(void);
const char* cpuid_str(int flag);

EXTERN_C_END

#endif
