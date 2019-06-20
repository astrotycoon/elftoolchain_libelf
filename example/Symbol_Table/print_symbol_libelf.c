#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

static const char *st_type(unsigned int stype) 
{
	switch (stype) {
		case STT_NOTYPE: return "NOTYPE";
		case STT_OBJECT: return "OBJECT";
		case STT_FUNC: return "FUNC";
		case STT_SECTION: return "SECTION";
		case STT_FILE: return "FILE";
		case STT_COMMON: return "COMMON";
		case STT_TLS: return "TLS";
		case STT_LOOS ... STT_HIOS: return "OS_SPEC";
		case STT_LOPROC ... STT_HIPROC: return "PROC_SPEC";
		default: return "<unknown symbol type>";
	}
}

static const char *st_bind(unsigned int stbind)
{
	switch (stbind) {
		case STB_LOCAL: return "LOCAL";
		case STB_GLOBAL: return "GLOBAL";
		case STB_WEAK: return "WEAK";
//		case STB_GNU_UNIQUE: return "UNIQUE";
		case STB_LOOS ... STB_HIOS: return "OS";
		case STB_LOPROC ... STB_HIPROC: return "PROC";
		default: return "<unknown symbol bind>";
	}
}

static const char *st_vis(unsigned int svis)
{
	switch (svis) {
		case STV_DEFAULT: return "DEFAULT";
		case STV_INTERNAL: return "INTERNAL";
		case STV_HIDDEN: return "HIDDEN";
		case STV_PROTECTED: return "PROTECTED";
		default: return "<unknown symbol vis>";
	}
}

static const char *st_shndx(unsigned int shndx)
{
	static char s_shndx[32];

	switch (shndx) {
		case SHN_UNDEF: return "UND";	
		case SHN_ABS: return "ABS";
		case SHN_COMMON: return "COMMON";
		case SHN_LOPROC ... SHN_HIPROC: return "PRC";
		case SHN_LOOS ... SHN_HIOS: return "OS";
		default:  
			(void)snprintf(s_shndx, sizeof(s_shndx), "%u", shndx); 
			return (const char *)s_shndx;
	}
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

static void print_syms(Elf *pelf, Elf_Scn *symscn, GElf_Shdr *symshdr)
{
	Elf_Data *symdata;

	if ((symdata = elf_getdata(symscn, NULL)) == NULL) {
		errx(EXIT_FAILURE, "elf_getdata() failed: %s.", elf_errmsg(-1));
	}

	if (symdata->d_type != ELF_T_SYM) {
		errx(EXIT_FAILURE, "Elf_Type is not ELF_T_SYM.");	
	}

	if (symdata->d_size <= 0) {
		errx(EXIT_FAILURE, "Section data size is wrong.");	
	}

	assert(symshdr->sh_size == symdata->d_size);

	// get shstrndx
	size_t shstrndx;
	if (elf_getshdrstrndx(pelf, &shstrndx) == -1) {
		errx(EXIT_FAILURE, "getshdrstrndx() failed: %s.", elf_errmsg(-1));	
	}

	const char *symshdrname = elf_strptr(pelf, shstrndx, symshdr->sh_name);
	if (symshdrname == NULL) {
		errx(EXIT_FAILURE, "elf_strptr() failed: %s,", elf_errmsg(-1));	
	}

	size_t entries; 

#if 0
	// 1
	entries = symshdr->sh_size / symshdr->sh_entsize;
#endif

#if 0
	// 2 
	size_t entsize = gelf_fsize(pelf, ELF_T_SYM, 1, EV_CURRENT); 
	entries = symshdr->sh_size / entsize;
#endif
	
#if 1
	// 3
	size_t entsize = gelf_fsize(pelf, ELF_T_SYM, 1, EV_CURRENT); 
	entries = symdata->d_size / entsize;
#endif

	printf("\nSymbol table '%s' contains %zu entries:\n", symshdrname, entries);
	printf("%7s%9s%14s%5s%8s%6s%9s%5s\n", "Num:", "Value", "Size", "Type",
	    "Bind", "Vis", "Ndx", "Name");

	for (size_t i = 0; i < entries; i++) {
		GElf_Sym sym;
		if (gelf_getsym(symdata, i, &sym) != &sym) {
			errx(EXIT_FAILURE, "gelf_getsym() failed: %s.", elf_errmsg(-1));
		}

		printf("%6zu:", i);
		printf(" %16.16jx", (uintmax_t)sym.st_value);
		printf(" %5ju", (uintmax_t)sym.st_size);
		printf(" %-7s", st_type(GELF_ST_TYPE(sym.st_info)));
		printf(" %-6s", st_bind(GELF_ST_BIND(sym.st_info)));
		printf(" %-8s", st_vis(GELF_ST_VISIBILITY(sym.st_other)));
		printf(" %3s", st_shndx(sym.st_shndx));
		// sh_info: One greater than the symbol table index of 
		// 			the last local symbol (binding STB_LOCAL).
		// printf("shdr->sh_info = %u\n", shdr->sh_info);
		// sh_link: .strtab or .dynstr (The section header index of 
		// 			the associated string table.)
		printf(" %s\n", get_symbol_name(pelf, symshdr->sh_link, &sym));	
	}
}

int main(int argc, const char *argv[])
{
	int fd, class;
	Elf *pelf = NULL;
	Elf_Scn *scn = NULL;

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

		if (shdr.sh_type == SHT_SYMTAB || shdr.sh_type == SHT_DYNSYM) {
			print_syms(pelf, scn, &shdr);
		}
	}	

	(void)elf_end(pelf);
	(void)close(fd);

	exit(EXIT_SUCCESS);
}
