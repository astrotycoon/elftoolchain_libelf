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

static void print_syms(Elf *pelf, const char *shname, Elf_Scn *scn, size_t entries, size_t strndx)
{
	Elf_Data *data;
	
	if ((data = elf_getdata(scn, NULL)) == NULL) {
		errx(EXIT_FAILURE, "elf_getdata() failed: %s.", elf_errmsg(-1));	
	}

	if (data->d_type != ELF_T_SYM) {
		errx(EXIT_FAILURE, "Elf_Type is not ELF_T_SYM.");	
	}

	if (data->d_size <= 0) {
		errx(EXIT_FAILURE, "Section data size is wrong.");	
	}

	printf("Symbol table '%s' contains %zu entries:\n", shname, entries);
	printf("%7s%9s%14s%5s%8s%6s%9s%5s\n", "Num:", "Value", "Size", "Type",
	    "Bind", "Vis", "Ndx", "Name");

	for (size_t i = 0; i < entries; i++) {
		GElf_Sym sym;
		if (gelf_getsym(data, i, &sym) != &sym) {
			errx(EXIT_FAILURE, "gelf_getsym() failed: %s.", elf_errmsg(-1));
		}

		printf("%6zu:", i);
		printf(" %16.16jx", (uintmax_t)sym.st_value);
		printf(" %5ju", (uintmax_t)sym.st_size);
		printf(" %-7s", st_type(ELF64_ST_TYPE(sym.st_info)));
		printf(" %-6s", st_bind(ELF64_ST_BIND(sym.st_info)));
		printf(" %-8s", st_vis(ELF64_ST_VISIBILITY(sym.st_other)));
		printf(" %3s", st_shndx(sym.st_shndx));
		printf(" %s", elf_strptr(pelf, strndx, sym.st_name));
		printf("\n");
	}
}

int main(int argc, const char *argv[])
{
	int fd, class;
	Elf *pelf = NULL;
	Elf_Scn *scn = NULL;
	size_t shstrndx;
	const char *shname;

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

	// get shstrndx
	if (elf_getshdrstrndx(pelf, &shstrndx) == -1) {
		errx(EXIT_FAILURE, "getshdrstrndx() failed: %s.", elf_errmsg(-1));	
	}

	while (scn = elf_nextscn(pelf, scn)) {
		GElf_Shdr shdr;
		if (gelf_getshdr(scn, &shdr) != &shdr) {
			errx(EXIT_FAILURE, "gelf_getshdr() failed: %s.", elf_errmsg(-1));	
		}

		if (shdr.sh_type == SHT_SYMTAB || shdr.sh_type == SHT_DYNSYM) {
			if (!(shname = elf_strptr(pelf, shstrndx, shdr.sh_name))) {
				errx(EXIT_FAILURE, "elf_strptr() failed: %s.", elf_errmsg(-1));	
			}
			size_t entries = shdr.sh_size / shdr.sh_entsize;
			// sh_link -> .strtab or .dynstr 
			print_syms(pelf, shname, scn, entries, shdr.sh_link);
		}
	}	

	(void)elf_end(pelf);
	(void)close(fd);

	exit(EXIT_SUCCESS);
}
