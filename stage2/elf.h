#ifndef ELF_H
#define ELF_H

#include <stdint.h>
#include <stdbool.h>

#define ELF_NIDENT  16
#define ELF_MAGIC   0x464c457F
#define ELF_LSB     (1)
#define ELFCLASS32  (1)
#define	EM_X86      0x03
#define SHN_UNDEF   0x00

// ELF Header
struct  Elf32_Ehdr {
	uint8_t  e_ident[ELF_NIDENT]; // Magic, bits, endianness, version info
	uint16_t e_type;              // Relocatable, Executable, Shared or Core
	uint16_t e_machine;           // Instruction Set
	uint32_t e_version;           // ELF version
	uint32_t e_entry;             // Entry point
	uint32_t e_phoff;             // Program Header Table offset
	uint32_t e_shoff;             // Section Header Table offset
	uint32_t e_flags;             // Flags
	uint16_t e_ehsize;            // Header size
	uint16_t e_phentsize;         // Size of program header entry
	uint16_t e_phnum;             // Number of program headers
	uint16_t e_shentsize;         // Size of section header entry
	uint16_t e_shnum;             // Number of section headers
	uint16_t e_shstrndx;          // Index in section header table with section names
};

// Section Header
struct Elf32_Shdr {
	uint32_t sh_name;       // Section Name
	uint32_t sh_type;       // Section Type
	uint32_t sh_flags;      // Section Flags
	uint32_t sh_addr;       // Load address
	uint32_t sh_offset;     // Offset into ELF file
	uint32_t sh_size;       // Size of segment
	uint32_t sh_link;       // Section header table index link
	uint32_t sh_info;       // Extra information
	uint32_t sh_addralign;  // Alignment of sh_addr
	uint32_t sh_entsize;    // Size of 1 element in entry table
};

// Program Header
struct Elf32_Phdr {
	uint32_t p_type;        // Segment Type
	uint32_t p_offset;      // Offset into ELF file
	uint32_t p_vaddr;       // Virtual load address
	uint32_t p_paddr;       // Physical load address
	uint32_t p_filesz;      // Size of segment in ELF file
	uint32_t p_memsz;       // Size of segment in RAM
	uint32_t p_flags;       // Flags
	uint32_t p_align;       // Alignment
};

enum Elf_Ident {
	EI_MAGIC      = 0,
	EI_CLASS      = 4,
	EI_DATA       = 5,
	EI_VERSION    = 6,
	EI_OSABI      = 7,
	EI_ABIVERSION = 8,
	EI_PAD        = 9
};

enum Elf_Type {
	ET_NONE = 0,
	ET_REL  = 1,
	ET_EXEC = 2,
	ET_DYN  = 3,
	ET_CORE = 4
};

enum Elf_PType {
	PT_NULL = 0,
	PT_LOAD = 1
};

enum Elf_ShType {
	SHT_NULL     = 0,
	SHT_PROGBITS = 1,
	SHT_SYMTAB   = 2,
	SHT_STRTAB   = 3,
	SHT_RELA     = 4,
	SHT_NOBITS   = 8,
	SHT_REL      = 9
};

enum Elf_ShAttr {
	SHF_WRITE = 0x01,
	SHF_ALLOC = 0x02
};

struct Elf_Headers {
	struct Elf32_Ehdr eh;
	struct Elf32_Phdr ph;
	struct Elf32_Shdr sh;
};

bool elf_is_elf(struct Elf32_Ehdr* hdr);
bool elf_is_supported(struct Elf32_Ehdr* hdr);
// buf must be large enough to store 1 ELF header, 1 Section Header, and 1 Program Header
void* elf_load_file(const char* filename, void* buf, int bufsiz);

#endif
