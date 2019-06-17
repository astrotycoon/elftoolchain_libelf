#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <link.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <elf.h>
#include <error.h>
#include <errno.h>

static const char *r_type(unsigned int machine, unsigned int type)
{
	switch (machine) {
	case EM_386:
		switch (type) {
		case 0: return "R_386_NONE";
		case 1: return "R_386_32";
		case 2: return "R_386_PC32";
		case 3: return "R_386_GOT32";
		case 4: return "R_386_PLT32";
		case 5: return "R_386_COPY";
		case 6: return "R_386_GLOB_DAT";
		case 7: return "R_386_JUMP_SLOT";
		case 8: return "R_386_RELATIVE";
		case 9: return "R_386_GOTOFF";
		case 10: return "R_386_GOTPC";
		case 11: return "R_386_32PLT"; /* Not in psabi */
		case 14: return "R_386_TLS_TPOFF";
		case 15: return "R_386_TLS_IE";
		case 16: return "R_386_TLS_GOTIE";
		case 17: return "R_386_TLS_LE";
		case 18: return "R_386_TLS_GD";
		case 19: return "R_386_TLS_LDM";
		case 20: return "R_386_16";
		case 21: return "R_386_PC16";
		case 22: return "R_386_8";
		case 23: return "R_386_PC8";
		case 24: return "R_386_TLS_GD_32";
		case 25: return "R_386_TLS_GD_PUSH";
		case 26: return "R_386_TLS_GD_CALL";
		case 27: return "R_386_TLS_GD_POP";
		case 28: return "R_386_TLS_LDM_32";
		case 29: return "R_386_TLS_LDM_PUSH";
		case 30: return "R_386_TLS_LDM_CALL";
		case 31: return "R_386_TLS_LDM_POP";
		case 32: return "R_386_TLS_LDO_32";
		case 33: return "R_386_TLS_IE_32";
		case 34: return "R_386_TLS_LE_32";
		case 35: return "R_386_TLS_DTPMOD32";
		case 36: return "R_386_TLS_DTPOFF32";
		case 37: return "R_386_TLS_TPOFF32";
		case 38: return "R_386_SIZE32";
		case 39: return "R_386_TLS_GOTDESC";
		case 40: return "R_386_TLS_DESC_CALL";
		case 41: return "R_386_TLS_DESC";
		case 42: return "R_386_IRELATIVE";
		case 43: return "R_386_GOT32X";
		}
		break;
	case EM_AARCH64:
		switch(type) {
		}
		break;
	case EM_ARM:
		switch(type) {
		case 0: return "R_ARM_NONE";
		case 1: return "R_ARM_PC24"; /* Deprecated */
		case 2: return "R_ARM_ABS32";
		case 3: return "R_ARM_REL32";
		case 4: return "R_ARM_LDR_PC_G0"; /* Also R_ARM_PC13 */
		case 5: return "R_ARM_ABS16";
		case 6: return "R_ARM_ABS12";
		case 7: return "R_ARM_THM_ABS5";
		case 8: return "R_ARM_ABS8";
		case 9: return "R_ARM_SBREL32";
		case 10: return "R_ARM_THM_CALL"; /* Also R_ARM_THM_PC22 */
		case 11: return "R_ARM_THM_PC8";
		case 12: return "R_ARM_BREL_ADJ"; /* Also R_ARM_AMP_VCALL9 */
		case 13: return "R_ARM_TLS_DESC"; /* Also R_ARM_SWI24 */
		case 14: return "R_ARM_THM_SWI8"; /* Obsolete */
		case 15: return "R_ARM_XPC25"; /* Obsolete */
		case 16: return "R_ARM_THM_XPC22"; /* Obsolete */
		case 17: return "R_ARM_TLS_DTPMOD32";
		case 18: return "R_ARM_TLS_DTPOFF32";
		case 19: return "R_ARM_TLS_TPOFF32";
		case 20: return "R_ARM_COPY";
		case 21: return "R_ARM_GLOB_DAT";
		case 22: return "R_ARM_JUMP_SLOT";
		case 23: return "R_ARM_RELATIVE";
		case 24: return "R_ARM_GOTOFF32"; /* Also R_ARM_GOTOFF */
		case 25: return "R_ARM_BASE_PREL"; /* GNU R_ARM_GOTPC */
		case 26: return "R_ARM_GOT_BREL"; /* GNU R_ARM_GOT32 */
		case 27: return "R_ARM_PLT32"; /* Deprecated */
		case 28: return "R_ARM_CALL";
		case 29: return "R_ARM_JUMP24";
		case 30: return "R_ARM_THM_JUMP24";
		case 31: return "R_ARM_BASE_ABS";
		case 32: return "R_ARM_ALU_PCREL_7_0"; /* Obsolete */
		case 33: return "R_ARM_ALU_PCREL_15_8"; /* Obsolete */
		case 34: return "R_ARM_ALU_PCREL_23_15"; /* Obsolete */
		case 35: return "R_ARM_LDR_SBREL_11_0_NC"; /* Deprecated */
		case 36: return "R_ARM_ALU_SBREL_19_12_NC"; /* Deprecated */
		case 37: return "R_ARM_ALU_SBREL_27_20_CK"; /* Deprecated */
		case 38: return "R_ARM_TARGET1";
		case 39: return "R_ARM_SBREL31"; /* Deprecated. */
		case 40: return "R_ARM_V4BX";
		case 41: return "R_ARM_TARGET2";
		case 42: return "R_ARM_PREL31";
		case 43: return "R_ARM_MOVW_ABS_NC";
		case 44: return "R_ARM_MOVT_ABS";
		case 45: return "R_ARM_MOVW_PREL_NC";
		case 46: return "R_ARM_MOVT_PREL";
		case 47: return "R_ARM_THM_MOVW_ABS_NC";
		case 48: return "R_ARM_THM_MOVT_ABS";
		case 49: return "R_ARM_THM_MOVW_PREL_NC";
		case 50: return "R_ARM_THM_MOVT_PREL";
		case 51: return "R_ARM_THM_JUMP19";
		case 52: return "R_ARM_THM_JUMP6";
		case 53: return "R_ARM_THM_ALU_PREL_11_0";
		case 54: return "R_ARM_THM_PC12";
		case 55: return "R_ARM_ABS32_NOI";
		case 56: return "R_ARM_REL32_NOI";
		case 57: return "R_ARM_ALU_PC_G0_NC";
		case 58: return "R_ARM_ALU_PC_G0";
		case 59: return "R_ARM_ALU_PC_G1_NC";
		case 60: return "R_ARM_ALU_PC_G1";
		case 61: return "R_ARM_ALU_PC_G2";
		case 62: return "R_ARM_LDR_PC_G1";
		case 63: return "R_ARM_LDR_PC_G2";
		case 64: return "R_ARM_LDRS_PC_G0";
		case 65: return "R_ARM_LDRS_PC_G1";
		case 66: return "R_ARM_LDRS_PC_G2";
		case 67: return "R_ARM_LDC_PC_G0";
		case 68: return "R_ARM_LDC_PC_G1";
		case 69: return "R_ARM_LDC_PC_G2";
		case 70: return "R_ARM_ALU_SB_G0_NC";
		case 71: return "R_ARM_ALU_SB_G0";
		case 72: return "R_ARM_ALU_SB_G1_NC";
		case 73: return "R_ARM_ALU_SB_G1";
		case 74: return "R_ARM_ALU_SB_G2";
		case 75: return "R_ARM_LDR_SB_G0";
		case 76: return "R_ARM_LDR_SB_G1";
		case 77: return "R_ARM_LDR_SB_G2";
		case 78: return "R_ARM_LDRS_SB_G0";
		case 79: return "R_ARM_LDRS_SB_G1";
		case 80: return "R_ARM_LDRS_SB_G2";
		case 81: return "R_ARM_LDC_SB_G0";
		case 82: return "R_ARM_LDC_SB_G1";
		case 83: return "R_ARM_LDC_SB_G2";
		case 84: return "R_ARM_MOVW_BREL_NC";
		case 85: return "R_ARM_MOVT_BREL";
		case 86: return "R_ARM_MOVW_BREL";
		case 87: return "R_ARM_THM_MOVW_BREL_NC";
		case 88: return "R_ARM_THM_MOVT_BREL";
		case 89: return "R_ARM_THM_MOVW_BREL";
		case 90: return "R_ARM_TLS_GOTDESC";
		case 91: return "R_ARM_TLS_CALL";
		case 92: return "R_ARM_TLS_DESCSEQ";
		case 93: return "R_ARM_THM_TLS_CALL";
		case 94: return "R_ARM_PLT32_ABS";
		case 95: return "R_ARM_GOT_ABS";
		case 96: return "R_ARM_GOT_PREL";
		case 97: return "R_ARM_GOT_BREL12";
		case 98: return "R_ARM_GOTOFF12";
		case 99: return "R_ARM_GOTRELAX";
		case 100: return "R_ARM_GNU_VTENTRY";
		case 101: return "R_ARM_GNU_VTINHERIT";
		case 102: return "R_ARM_THM_JUMP11"; /* Also R_ARM_THM_PC11 */
		case 103: return "R_ARM_THM_JUMP8"; /* Also R_ARM_THM_PC9 */
		case 104: return "R_ARM_TLS_GD32";
		case 105: return "R_ARM_TLS_LDM32";
		case 106: return "R_ARM_TLS_LDO32";
		case 107: return "R_ARM_TLS_IE32";
		case 108: return "R_ARM_TLS_LE32";
		case 109: return "R_ARM_TLS_LDO12";
		case 110: return "R_ARM_TLS_LE12";
		case 111: return "R_ARM_TLS_IE12GP";
		/* 112-127 R_ARM_PRIVATE_<n> */
		case 128: return "R_ARM_ME_TOO"; /* Obsolete */
		case 129: return "R_ARM_THM_TLS_DESCSEQ16";
		case 130: return "R_ARM_THM_TLS_DESCSEQ32";
		case 131: return "R_ARM_THM_GOT_BREL12";
		case 132: return "R_ARM_THM_ALU_ABS_G0_NC";
		case 133: return "R_ARM_THM_ALU_ABS_G1_NC";
		case 134: return "R_ARM_THM_ALU_ABS_G2_NC";
		case 135: return "R_ARM_THM_ALU_ABS_G3";
		/* 136-159 Reserved for future allocation. */
		case 160: return "R_ARM_IRELATIVE";
		/* 161-255 Reserved for future allocation. */
		case 249: return "R_ARM_RXPC25";
		case 250: return "R_ARM_RSBREL32";
		case 251: return "R_ARM_THM_RPC22";
		case 252: return "R_ARM_RREL32";
		case 253: return "R_ARM_RABS32";
		case 254: return "R_ARM_RPC24";
		case 255: return "R_ARM_RBASE";
		}
		break;
	case EM_IA_64:
		switch(type) {
		}
		break;
	case EM_MIPS:
		switch(type) {
		}
		break;
	case EM_PPC:
		switch(type) {
		}
		break;
	case EM_PPC64:
		switch(type) {
		}
		break;
	case EM_RISCV:
		switch(type) {
		}
		break;
	case EM_S390:
		switch (type) {
		}
		break;
	case EM_SPARC:
	case EM_SPARCV9:
		switch(type) {
		}
		break;
	case EM_X86_64:
		switch (type) {
		case 0: return "R_X86_64_NONE";
		case 1: return "R_X86_64_64";
		case 2: return "R_X86_64_PC32";
		case 3: return "R_X86_64_GOT32";
		case 4: return "R_X86_64_PLT32";
		case 5: return "R_X86_64_COPY";
		case 6: return "R_X86_64_GLOB_DAT";
		case 7: return "R_X86_64_JUMP_SLOT";
		case 8: return "R_X86_64_RELATIVE";
		case 9: return "R_X86_64_GOTPCREL";
		case 10: return "R_X86_64_32";
		case 11: return "R_X86_64_32S";
		case 12: return "R_X86_64_16";
		case 13: return "R_X86_64_PC16";
		case 14: return "R_X86_64_8";
		case 15: return "R_X86_64_PC8";
		case 16: return "R_X86_64_DTPMOD64";
		case 17: return "R_X86_64_DTPOFF64";
		case 18: return "R_X86_64_TPOFF64";
		case 19: return "R_X86_64_TLSGD";
		case 20: return "R_X86_64_TLSLD";
		case 21: return "R_X86_64_DTPOFF32";
		case 22: return "R_X86_64_GOTTPOFF";
		case 23: return "R_X86_64_TPOFF32";
		case 24: return "R_X86_64_PC64";
		case 25: return "R_X86_64_GOTOFF64";
		case 26: return "R_X86_64_GOTPC32";
		case 27: return "R_X86_64_GOT64";
		case 28: return "R_X86_64_GOTPCREL64";
		case 29: return "R_X86_64_GOTPC64";
		case 30: return "R_X86_64_GOTPLT64";
		case 31: return "R_X86_64_PLTOFF64";
		case 32: return "R_X86_64_SIZE32";
		case 33: return "R_X86_64_SIZE64";
		case 34: return "R_X86_64_GOTPC32_TLSDESC";
		case 35: return "R_X86_64_TLSDESC_CALL";
		case 36: return "R_X86_64_TLSDESC";
		case 37: return "R_X86_64_IRELATIVE";
		case 38: return "R_X86_64_RELATIVE64";
		case 41: return "R_X86_64_GOTPCRELX";
		case 42: return "R_X86_64_REX_GOTPCRELX";
		}
		break;
	}
	return NULL;
}

static void print_rel(ElfW(Rel) *rels, size_t reltab_entries, 
		ElfW(Sym) *symtab, size_t symtab_entries, 
		const char *strtab, int class, int machine)
{
	printf("%-16s %-16s %-24s %-16s %s\n", 
			"r_offset", "r_info", "r_type", "st_value", "st_name");

	for (size_t i = 0; i < reltab_entries; i++) {
		ElfW(Rel) *rel = &rels[i];	
		printf("%16.16jx", (uintmax_t)rel->r_offset);	
		printf(" %16.16jx", (uintmax_t)rel->r_info);
		printf(" %-24.24s", class == ELFCLASS32 
					? r_type(machine, ELF32_R_TYPE(rel->r_info)) 
					: r_type(machine, ELF64_R_TYPE(rel->r_info)));

		size_t symtabndx;
		if (class == ELFCLASS32) {
			symtabndx = ELF32_R_SYM(rel->r_info);
		} else {
			symtabndx = ELF64_R_SYM(rel->r_info);
		}
		if (symtabndx >= symtab_entries) {
			(void)fprintf(stderr, "Symbol index out of range.\n");	
			continue;
		}
		printf(" %16.16jx", (uintmax_t)symtab[symtabndx].st_value);
		printf(" %s\n", strtab + symtab[symtabndx].st_name);
	}
}

static void print_rela(ElfW(Rela) *relas, size_t reltab_entries, 
		ElfW(Sym) *symtab, size_t symtab_entries, 
		const char *strtab, int class, int machine)
{
	printf("%-16s %-16s %-24s %-16s %s\n", 
			"r_offset", "r_info", "r_type", "st_value", "st_name + r_addend");

	for (size_t i = 0; i < reltab_entries; i++) {
		ElfW(Rela) *rela = &relas[i];	
		printf("%16.16jx", (uintmax_t)rela->r_offset);	
		printf(" %16.16jx", (uintmax_t)rela->r_info);
		printf(" %-24.24s", class == ELFCLASS32 
					? r_type(machine, ELF32_R_TYPE(rela->r_info)) 
					: r_type(machine, ELF64_R_TYPE(rela->r_info)));

		size_t symtabndx;
		if (class == ELFCLASS32) {
			symtabndx = ELF32_R_SYM(rela->r_info);
		} else {
			symtabndx = ELF64_R_SYM(rela->r_info);
		}
		if (symtabndx >= symtab_entries) {
			(void)fprintf(stderr, "Symbol index out of range.\n");	
			continue;
		}
		printf(" %16.16jx", (uintmax_t)symtab[symtabndx].st_value);
		printf(" %s", strtab + symtab[symtabndx].st_name);
		printf(" + %jx\n", (uintmax_t)rela->r_addend);
	}
}
 
int main(int argc, char *argv[])
{
	int fd;
	char *file_mmbase;
	struct stat file_status;
	size_t fsize;
	ElfW(Ehdr) *ehdr;
	ElfW(Shdr) *shdrs;
	size_t shnum, shstrndx;
	int class, machine;

//	[ 9] .rela.dyn         RELA            0000000000000560 000560 0000c0 18   A  5   0  8
//	[10] .rela.plt         RELA            0000000000000620 000620 000108 18  AI  5  22  8
 
	if (argc != 2) {
		error(EXIT_FAILURE, 0, "Usage: %s file-name", argv[0]);
	}
 
	if ((fd = open(argv[1], O_RDONLY)) < 0) {
		error(EXIT_FAILURE, errno, "open %s failed", argv[1]);
	}
 
	if (fstat(fd, &file_status) < 0) {
		error(EXIT_FAILURE, errno, "get file %s info err", argv[1]);
	}
	fsize = (size_t)file_status.st_size;
 
	if ((file_mmbase = mmap(NULL, fsize, PROT_READ, 
				MAP_PRIVATE, fd, (off_t)0)) == MAP_FAILED) {
		error(EXIT_FAILURE, errno, "mmap file %s err", argv[1]);
	}
 
	ehdr = (ElfW(Ehdr) *)file_mmbase;
	shdrs = (ElfW(Shdr) *)(file_mmbase + ehdr->e_shoff);
	shnum = ehdr->e_shnum == 0 ? shdrs[0].sh_size : ehdr->e_shnum;
	shstrndx = ehdr->e_shstrndx == SHN_XINDEX ? shdrs[0].sh_link : ehdr->e_shstrndx;
	class = ehdr->e_ident[EI_CLASS];
	machine = ehdr->e_machine; 

	for (size_t i = 0; i < shnum; i++) {
		ElfW(Shdr) *shdr = &shdrs[i];	

		const char *shname = file_mmbase + shdrs[shstrndx].sh_offset + shdr->sh_name;

		if (shdr->sh_type == SHT_REL || shdr->sh_type == SHT_RELA) {

  			// Get the symbol table information. 
			// for SHT_REL or SHT_RELA:
			// 		sh_link: The section header index 
			// 				of the associated symbol table.
			// 		sh_info: The section header index 
			// 		        of the section to which the relocation applies.
			size_t symtabndx = shdr->sh_link;
			if (shdrs[symtabndx].sh_type != SHT_SYMTAB
					&& shdrs[symtabndx].sh_type != SHT_DYNSYM) {
				(void)fprintf(stderr, "Associated section is not symbol table.\n");	
				continue;
			}
			ElfW(Sym) *symtab = (ElfW(Sym *))(file_mmbase + shdrs[symtabndx].sh_offset);
			size_t symtab_entries = shdrs[symtabndx].sh_size / shdrs[symtabndx].sh_entsize;
			// for SHT_SYMTAB or SHT_DYNSYM
			// 		sh_info: One greater than the symbol table index 
			// 				 of the last local symbol (binding STB_LOCAL).
			// 		sh_link: The section header index 
			// 		         of the associated string table.
			size_t strtabndx = shdrs[symtabndx].sh_link;
			const char *strtab = file_mmbase + shdrs[strtabndx].sh_offset; 

			size_t reltab_entries = shdr->sh_size / shdr->sh_entsize;

			if (shdr->sh_info != 0) {
				ElfW(Shdr) *destshdr = &shdrs[shdr->sh_info];
				const char *destshdrname = file_mmbase 
							+ shdrs[shstrndx].sh_offset + destshdr->sh_name;
				(void)printf("\nRelocation section [%2zu] '%s' "
                    "for section [%2u] '%s' at offset 0x%jx contains %zu entries:\n",
					i, shname, shdr->sh_info, destshdrname,
					(uintmax_t)shdr->sh_offset, reltab_entries);
			} else {
				// The .rel.dyn section does not refer to a specific section	
				(void)printf("\nRelocation section [%2zu] '%s' "
                    "at offset 0x%jx contains %zu entries:\n",
					i, shname, (uintmax_t)shdr->sh_offset, reltab_entries);
			}

			if (shdr->sh_type == SHT_REL) {
				ElfW(Rel) *rels = (ElfW(Rel) *)(file_mmbase + shdr->sh_offset);
				print_rel(rels, reltab_entries, 
			        symtab, symtab_entries, strtab, class, machine);
			}

			if (shdr->sh_type == SHT_RELA) {
				ElfW(Rela) *relas = (ElfW(Rela) *)(file_mmbase + shdr->sh_offset);
				print_rela(relas, reltab_entries, 
			        symtab, symtab_entries, strtab, class, machine);	
			}
		}
	}

	(void)munmap(file_mmbase, fsize);
	(void)close(fd);
	
	exit(EXIT_SUCCESS);
}

