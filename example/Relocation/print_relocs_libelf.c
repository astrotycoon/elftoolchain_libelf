#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

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

static const char *get_r_type(Elf *pelf, unsigned int type)
{
	GElf_Ehdr ehdr;
	if (gelf_getehdr(pelf, &ehdr) != &ehdr) {
		errx(EXIT_FAILURE, "gelf_getehdr() failed: %s.", elf_errmsg(-1));	
	}

	return r_type(ehdr.e_machine, type);
}

static const char *get_symbol_name(Elf *pelf, size_t strtabndx, GElf_Sym *sym)
{
	if (GELF_ST_TYPE(sym->st_info) == STT_SECTION) {
		// get shstrndx
		size_t shstrndx;
		if (elf_getshdrstrndx(pelf, &shstrndx) == -1) {
			errx(EXIT_FAILURE, "getshdrstrndx() failed: %s.", elf_errmsg(-1));
		}
	
		Elf_Scn *destscn;
		GElf_Shdr destshdr;
	   
		if ((destscn = elf_getscn(pelf, sym->st_shndx)) == NULL) {
			errx(EXIT_FAILURE, "elf_getscn() failed: %s.", elf_errmsg(-1));
		}
		if (gelf_getshdr(destscn, &destshdr) != &destshdr) {
			errx(EXIT_FAILURE, "gelf_getshdr() failed: %s.", elf_errmsg(-1));
		}
		return elf_strptr(pelf, shstrndx, destshdr.sh_name);
	}
	
	return  elf_strptr(pelf, strtabndx, sym->st_name);
}

static void print_rel(Elf *pelf, Elf_Scn *relscn, GElf_Shdr *relshdr)
{
	size_t sh_entsize = gelf_fsize(pelf, ELF_T_REL, 1, EV_CURRENT);
	size_t entries = relshdr->sh_size / sh_entsize;

	Elf_Data *reldata = elf_getdata(relscn, NULL);
	if (reldata == NULL) return;

	// Get the symbol table information.
	// for SHT_REL or SHT_RELA:
	// 	sh_link: The section header index
	// 			of the associated symbol table.
	// 	sh_info: The section header index
	// 			of the section to which the relocation applies.
	Elf_Scn *symscn = elf_getscn(pelf, relshdr->sh_link);
	if (symscn == NULL) return;

	GElf_Shdr symshdr;
	if (gelf_getshdr(symscn, &symshdr) != &symshdr) return;

	Elf_Data *symdata = elf_getdata(symscn, NULL);
	if (symdata == NULL) return;

	// Get the section header string table index.
	size_t shstrndx;
	if (elf_getshdrstrndx(pelf, &shstrndx) < 0) return;

	if (relshdr->sh_info != 0) {
		// Get the section header of the section the relocations are for.
		Elf_Scn *destscn = elf_getscn(pelf, relshdr->sh_info);	
		GElf_Shdr destshdr;
		if (gelf_getshdr(destscn, &destshdr) != &destshdr) {
			errx(EXIT_FAILURE,
					"gelf_getshdr() failed: %s.", elf_errmsg(-1));	
		}

		(void)printf("\nRelocation section [%2zu] '%s' "
			"for section [%2u] '%s' at offset 0x%jx contains %zu entries:\n",
			elf_ndxscn(relscn),
			elf_strptr(pelf, shstrndx, relshdr->sh_name),
			relshdr->sh_info,
			elf_strptr(pelf, shstrndx, destshdr.sh_name),
			relshdr->sh_offset, entries);
	} else {
		// The .rela.dyn section does not refer to a specific section
		(void)printf("\nRelocation section [%2zu] '%s' "
				"at offset 0x%jx contains %zu entries:\n",
				elf_ndxscn(relscn),
				elf_strptr(pelf, shstrndx, relshdr->sh_name),
				relshdr->sh_offset, entries);	
	}

	printf("%-16s %-16s %-24s %-16s %s\n", 
			"r_offset", "r_info", "r_type", "st_value", "st_name");


	for (size_t i = 0; i < entries; i++) {
		GElf_Rel rel;
		if (gelf_getrel(reldata, i, &rel) != &rel) {
			errx(EXIT_FAILURE, "gelf_getrel() failed: %s.", elf_errmsg(-1));	
		}

		printf("%16.16jx", (uintmax_t)rel.r_offset);	
		printf(" %16.16jx", (uintmax_t)rel.r_info);
		printf(" %-24.24s", get_r_type(pelf, GELF_R_TYPE(rel.r_info))); 

		GElf_Sym sym;
		if (gelf_getsym(symdata, GELF_R_SYM(rel.r_info), &sym) != &sym) {
			errx(EXIT_FAILURE, "gelf_getsym() failed: %s.", elf_errmsg(-1));	
		}
		printf(" %16.16jx", (uintmax_t)sym.st_value);
		// for SHT_SYMTAB or SHT_DYNSYM
		// 	sh_info: One greater than the symbol table index
		// 		of the last local symbol (binding STB_LOCAL) 
		//	sh_link: The section header index
		//		of the associated string table.
		printf(" %s\n", get_symbol_name(pelf, symshdr.sh_link, &sym));
	}
}

static void print_rela(Elf *pelf, Elf_Scn *relascn, GElf_Shdr *relashdr)
{
	size_t sh_entsize = gelf_fsize(pelf, ELF_T_RELA, 1, EV_CURRENT);
	size_t entries = relashdr->sh_size / sh_entsize;

	Elf_Data *reladata = elf_getdata(relascn, NULL);
	if (reladata == NULL) return;

	// Get the symbol table information.
	// for SHT_REL or SHT_RELA:
	// 	sh_link: The section header index
	// 			of the associated symbol table.
	// 	sh_info: The section header index
	// 			of the section to which the relocation applies.
	Elf_Scn *symscn = elf_getscn(pelf, relashdr->sh_link);
	if (symscn == NULL) return;

	GElf_Shdr symshdr;
	if (gelf_getshdr(symscn, &symshdr) != &symshdr) return;

	Elf_Data *symdata = elf_getdata(symscn, NULL);
	if (symdata == NULL) return;

	// Get the section header string table index.
	size_t shstrndx;
	if (elf_getshdrstrndx(pelf, &shstrndx) < 0) return;

	if (relashdr->sh_info != 0) {
		// Get the section header of the section the relocations are for.
		Elf_Scn *destscn = elf_getscn(pelf, relashdr->sh_info);	
		GElf_Shdr destshdr;
		if (gelf_getshdr(destscn, &destshdr) != &destshdr) {
			errx(EXIT_FAILURE,
					"gelf_getshdr() failed: %s.", elf_errmsg(-1));	
		}

		(void)printf("\nRelocation section [%2zu] '%s' "
			"for section [%2u] '%s' at offset 0x%jx contains %zu entries:\n",
			elf_ndxscn(relascn),
			elf_strptr(pelf, shstrndx, relashdr->sh_name),
			relashdr->sh_info,
			elf_strptr(pelf, shstrndx, destshdr.sh_name),
			relashdr->sh_offset, entries);
	} else {
		// The .rela.dyn section does not refer to a specific section
		(void)printf("\nRelocation section [%2zu] '%s' "
				"at offset 0x%jx contains %zu entries:\n",
				elf_ndxscn(relascn),
				elf_strptr(pelf, shstrndx, relashdr->sh_name),
				relashdr->sh_offset, entries);	
	}

	printf("%-16s %-16s %-24s %-16s %s\n", 
			"r_offset", "r_info", "r_type", "st_value", "st_name + r_addend");

	for (size_t i = 0; i < entries; i++) {
		GElf_Rela rela;
		if (gelf_getrela(reladata, i, &rela) != &rela) {
			errx(EXIT_FAILURE, "gelf_getrela() failed: %s.", elf_errmsg(-1));	
		}

		printf("%16.16jx", (uintmax_t)rela.r_offset);	
		printf(" %16.16jx", (uintmax_t)rela.r_info);
		printf(" %-24.24s", get_r_type(pelf, GELF_R_TYPE(rela.r_info))); 

		GElf_Sym sym;
		if (gelf_getsym(symdata, GELF_R_SYM(rela.r_info), &sym) != &sym) {
			errx(EXIT_FAILURE, "gelf_getsym() failed: %s.", elf_errmsg(-1));	
		}
		printf(" %16.16jx", (uintmax_t)sym.st_value);
		// for SHT_SYMTAB or SHT_DYNSYM
		// 	sh_info: One greater than the symbol table index
		// 		of the last local symbol (binding STB_LOCAL) 
		//	sh_link: The section header index
		//		of the associated string table.
		printf(" %s", get_symbol_name(pelf, symshdr.sh_link, &sym));
		printf(" + %jx\n", (uintmax_t)rela.r_addend);
	}
}

int main(int argc, const char *argv[])
{
	int fd, class;
	Elf *pelf = NULL;
	Elf_Scn *scn = NULL;

// [ 9] .rela.dyn         RELA            0000000000000568 000568 0000d8 18   A  5   0  8
// [10] .rela.plt         RELA            0000000000000640 000640 0000f0 18  AI  5  22  8

	if (argc != 2)
		errx(EXIT_FAILURE, "usage: %s file-name", argv[0]);

	if (elf_version(EV_CURRENT) == EV_NONE)
		errx(EXIT_FAILURE, "ELF library initializztion "
			"failed: %s", elf_errmsg(-1));

	if ((fd = open(argv[1], O_RDONLY, 0)) < 0)
		errx(EXIT_FAILURE, "open \"%s\" failed", argv[1]);

	if (!(pelf = elf_begin(fd, ELF_C_READ, NULL)))
		errx(EXIT_FAILURE, "elf_begin() failed: %s", elf_errmsg(-1));

	if (elf_kind(pelf) != ELF_K_ELF)
		errx(EXIT_FAILURE, "\"%s\" is not an ELF object.", argv[1]);

	// get elf class ()
	if ((class = gelf_getclass(pelf)) == ELFCLASSNONE)
		errx(EXIT_FAILURE, "getclass() failed: %s.", elf_errmsg(-1));

	while ((scn = elf_nextscn(pelf, scn)) != NULL) {
		GElf_Shdr shdr;
		if (gelf_getshdr(scn, &shdr) != &shdr) {
			errx(EXIT_FAILURE, "gelf_getshdr() failed: %s.", elf_errmsg(-1));
		}

		if (shdr.sh_type == SHT_REL) {
			print_rel(pelf, scn, &shdr);	
		}

		if (shdr.sh_type == SHT_RELA) {
			print_rela(pelf, scn, &shdr);
		}
	}	

	(void)elf_end(pelf);
	(void)close(fd);

	exit(EXIT_SUCCESS);
}
