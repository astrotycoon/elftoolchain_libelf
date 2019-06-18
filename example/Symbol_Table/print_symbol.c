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

#define ELFW(type)	_ELFW (ELF, __ELF_NATIVE_CLASS, type)
#define _ELFW(e,w,t)	_ELFW_1 (e, w, _##t)
#define _ELFW_1(e,w,t)	e##w##t

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

static void print_syms(ElfW(Shdr) *shdrs, const char *shstrtab,  
		const char *shname, ElfW(Sym) *syms, size_t entries, const char *strtab)
{
	printf("Symbol table '%s' contains %zu entries:\n", shname, entries);
	printf("%7s%9s%14s%5s%8s%6s%9s%5s\n", "Num:", "Value", "Size", "Type",
	    "Bind", "Vis", "Ndx", "Name");

	for (size_t i = 0; i < entries; i++) {
		ElfW(Sym) *sym = &syms[i];
	
		printf("%6zu:", i);
		printf(" %16.16jx", (uintmax_t)sym->st_value);
		printf(" %5ju", (uintmax_t)sym->st_size);
		printf(" %-7s", st_type(ELFW(ST_TYPE)(sym->st_info)));
		printf(" %-6s", st_bind(ELFW(ST_BIND)(sym->st_info)));
		printf(" %-8s", st_vis(ELFW(ST_VISIBILITY)(sym->st_other)));
		printf(" %3s", st_shndx(sym->st_shndx));
		if (strcmp("SECTION", st_type(ELFW(ST_TYPE)(sym->st_info))) == 0) {
			printf(" %s", shstrtab + shdrs[sym->st_shndx].sh_name);	
		} else {
			printf(" %s", strtab + sym->st_name);
		}
		printf("\n");
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
	const char *shstrtab;

//	[ 6] .dynstr           STRTAB          0000000000000468 000468 0000dd 00   A  0   0  1
//	[32] .strtab           STRTAB          0000000000000000 003d28 000322 00      0   0  1
//	[33] .shstrtab         STRTAB          0000000000000000 00404a 00013e 00      0   0  1
 
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
	shstrtab = file_mmbase + shdrs[shstrndx].sh_offset;

	for (size_t i = 0; i < shnum; i++) {
		ElfW(Shdr) *shdr = &shdrs[i];	

		if (shdr->sh_type == SHT_SYMTAB || shdr->sh_type == SHT_DYNSYM) {
			const char *shname = shstrtab + shdr->sh_name;
			ElfW(Sym) *syms = (ElfW(Sym *))(file_mmbase + shdr->sh_offset); 
			size_t entries = shdr->sh_size / shdr->sh_entsize;
			// sh_info: One greater than the symbol table index of 
			// 			the last local symbol (binding STB_LOCAL).
			// printf("shdr->sh_info = %u\n", shdr->sh_info);
			// sh_link: .strtab or .dynstr (The section header index of 
			// 			the associated string table.)
			const char *strtab = file_mmbase + shdrs[shdr->sh_link].sh_offset;
			print_syms(shdrs, shstrtab, shname, syms, entries, strtab);	
		}
	}

	(void)munmap(file_mmbase, fsize);
	(void)close(fd);
	
	exit(EXIT_SUCCESS);
}


