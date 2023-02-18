#include "elf.h"
#include "util.h"
#include "pt.h"
#include "pff3a/source/pff.h"

bool elf_is_elf(struct Elf32_Ehdr* hdr) {
	uint32_t elf_magic = ELF_MAGIC;
	return hdr && (memcmp(hdr, &elf_magic, 4) == 0);
}

bool elf_is_supported(struct Elf32_Ehdr* hdr) {
	if (!elf_is_elf(hdr)) {
		return false;
	} else if (hdr->e_ident[EI_CLASS] != ELFCLASS32) {
		return false;
	} else if (hdr->e_ident[EI_DATA] != ELF_LSB) {
		return false;
	} else if (hdr->e_machine != EM_X86) {
		return false;
	} else if (hdr->e_ident[EI_VERSION] != 1) {
		return false;
	} else if (hdr->e_type != ET_EXEC) { // Relocation not supported in bootloader
		return false;
	}
	return true;
}

struct Elf32_Phdr *elf_pheader(struct Elf32_Ehdr* hdr) {
	return (struct Elf32_Phdr*)(((uint32_t)hdr) + sizeof(struct Elf32_Phdr));
}

struct Elf32_Shdr *elf_sheader(struct Elf32_Ehdr* hdr) {
	return (struct Elf32_Shdr*)(((uint32_t)hdr) + sizeof(struct Elf32_Phdr) + sizeof(struct Elf32_Shdr));
}

void* elf_load_file(const char* filename, void* buf, int bufsiz) {
	size_t minsize = sizeof(struct Elf32_Ehdr) + sizeof(struct Elf32_Phdr) + sizeof(struct Elf32_Shdr);
	if (pf_open(filename) || (bufsiz < minsize)) {
		return NULL;
	}
	
	struct Elf32_Ehdr* hdr = buf;
	struct Elf32_Phdr* phdr = elf_pheader(buf);
	struct Elf32_Shdr* shdr = elf_sheader(buf);
	int bytesread = 0;
	int i = 0;
	size_t offset = 0;
	size_t remaining = 0;
	void* vaddr = NULL;
	
	if (pf_read(buf, sizeof(struct Elf32_Ehdr), &bytesread)) {
		return NULL;
	} else if (bytesread != sizeof(struct Elf32_Ehdr)) {
		return NULL;
	} else if (!elf_is_supported(hdr)) {
		return NULL;
	}
	
	for (i = 0; i < hdr->e_shnum; i++) {
		if (pf_lseek(hdr->e_shoff + (i * hdr->e_shentsize))) {
			return NULL;
		} else if (pf_read(shdr, sizeof(struct Elf32_Shdr), &bytesread)) {
			return NULL;
		} else if (bytesread != sizeof(struct Elf32_Shdr)) {
			return NULL;
		}
		
		if (shdr->sh_type == SHT_NOBITS) {
			if (!shdr->sh_size) continue;
			if (shdr->sh_flags & SHF_ALLOC) {
				for (bytesread = 0; bytesread <= shdr->sh_size; bytesread += 0x1000) {
					pt_map(shdr->sh_addr + bytesread);
				}
			}
		}
	}
	
	for (i = 0; i < hdr->e_phnum; i++) {
		if (pf_lseek(hdr->e_phoff + (i * hdr->e_phentsize))) {
			return NULL;
		} else if (pf_read(phdr, sizeof(struct Elf32_Phdr), &bytesread)) {
			return NULL;
		} else if (bytesread != sizeof(struct Elf32_Phdr)) {
			return NULL;
		}
		
		if (phdr->p_type == PT_LOAD) {
			pf_lseek(phdr->p_offset);
			for (bytesread = 0; bytesread <= phdr->p_filesz; bytesread += 0x1000) {
				pt_map(phdr->p_vaddr + bytesread);
			}
			pf_read((void*)(phdr->p_vaddr), phdr->p_filesz, &bytesread);
			memset((void*)(phdr->p_vaddr + bytesread), 0, phdr->p_memsz - bytesread);
		}

	}
	
	return (void*)(hdr->e_entry);
}
