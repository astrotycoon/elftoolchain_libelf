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

static void print_syms(const char *shname, ElfW(Sym) *syms, 
		size_t entries, const char *strtab)
{
	printf("Symbol table '%s' contains %zu entries:\n", shname, entries);
	printf("%7s%9s%14s%5s%8s%6s%9s%5s\n", "Num:", "Value", "Size", "Type",
	    "Bind", "Vis", "Ndx", "Name");

	for (size_t i = 0; i < entries; i++) {
		ElfW(Sym) *sym = &syms[i];
	
		printf("%6zu:", i);
		printf(" %16.16jx", (uintmax_t)sym->st_value);
		printf(" %5ju", (uintmax_t)sym->st_size);
		printf(" %-7s", st_type(ELF64_ST_TYPE(sym->st_info)));
		printf(" %-6s", st_bind(ELF64_ST_BIND(sym->st_info)));
		printf(" %-8s", st_vis(ELF64_ST_VISIBILITY(sym->st_other)));
		printf(" %3s", st_shndx(sym->st_shndx));
		printf(" %s", strtab + sym->st_name);
		printf("\n");
	}
}

static const char *r_type32(unsigned int type)
{}

static const char *r_type64(unsigned int type)
{
	switch (type) {
		case R_X86_64_NONE: return "R_X86_64_NONE";
		case R_X86_64_64: return "R_X86_64_64";
		case R_X86_64_PC32: return "R_X86_64_PC32";
		case R_X86_64_GOT32: return "R_X86_64_GOT32";
		case R_X86_64_PLT32: return "R_X86_64_PLT32";
		case R_X86_64_COPY: return "R_X86_64_COPY";
		case R_X86_64_GLOB_DAT: return "R_X86_64_GLOB_DAT";
		case R_X86_64_JUMP_SLOT: return "R_X86_64_JUMP_SLOT";
		case R_X86_64_RELATIVE: return "R_X86_64_RELATIVE";
		case R_X86_64_GOTPCREL: return "R_X86_64_GOTPCREL";
		case R_X86_64_32: return "R_X86_64_32";
		case R_X86_64_32S: return "R_X86_64_32S";
		case R_X86_64_16: return "R_X86_64_16";
		case R_X86_64_PC16: return "R_X86_64_PC16";
		case R_X86_64_8: return "R_X86_64_8";
		case R_X86_64_PC8: return "R_X86_64_PC8";
		case R_X86_64_DTPMOD64: return "R_X86_64_DTPMOD64";
		case R_X86_64_DTPOFF64: return "R_X86_64_DTPOFF64";
		case R_X86_64_TPOFF64: return "R_X86_64_TPOFF64";
		case R_X86_64_TLSGD: return "R_X86_64_TLSGD";
		case R_X86_64_TLSLD: return "R_X86_64_TLSLD";
		case R_X86_64_DTPOFF32: return "R_X86_64_DTPOFF32";
		case R_X86_64_GOTTPOFF: return "R_X86_64_GOTTPOFF";
		case R_X86_64_TPOFF32: return "R_X86_64_TPOFF32";
		case R_X86_64_PC64: return "R_X86_64_PC64";
		case R_X86_64_GOTOFF64: return "R_X86_64_GOTOFF64";
		case R_X86_64_GOTPC32: return "R_X86_64_GOTPC32";
		case R_X86_64_GOT64: return "R_X86_64_GOT64";
		case R_X86_64_GOTPCREL64: return "R_X86_64_GOTPCREL64";
		case R_X86_64_GOTPC64: return "R_X86_64_GOTPC64";
		case R_X86_64_GOTPLT64: return "R_X86_64_GOTPLT64";
		case R_X86_64_PLTOFF64: return "R_X86_64_PLTOFF64";
		case R_X86_64_SIZE32: return "R_X86_64_SIZE32";
		case R_X86_64_SIZE64: return "R_X86_64_SIZE64";
		case R_X86_64_GOTPC32_TLSDESC: return "R_X86_64_GOTPC32_TLSDESC";
		case R_X86_64_TLSDESC_CALL: return "R_X86_64_TLSDESC_CALL";
		case R_X86_64_TLSDESC: return "R_X86_64_TLSDESC";
		case R_X86_64_IRELATIVE: return "R_X86_64_IRELATIVE";
		case R_X86_64_RELATIVE64: return "R_X86_64_RELATIVE64";
		case R_X86_64_GOTPCRELX: return "R_X86_64_GOTPCRELX";
		case R_X86_64_REX_GOTPCRELX: return "R_X86_64_REX_GOTPCRELX";
	}
	return NULL;
}

static void print_rela(ElfW(Rela) *relas, size_t reltab_entries, 
		ElfW(Sym) *symtab, size_t symtab_entries, const char *strtab, int class)
{
	printf("%-17s %-14s %-14s %-13s %s\n", "  Offset", "Info", "Type", "Sym. Value", "Sym. Name + Addend");

	for (size_t i = 0; i < reltab_entries; i++) {
		ElfW(Rela) *rela = &relas[i];	
		printf("%012jx", (uintmax_t)rela->r_offset);	
		printf(" %012jx", (uintmax_t)rela->r_info);
		printf(" %-7s", class == ELFCLASS32 ? 
					r_type32(ELF32_R_TYPE(rela->r_info)) : 
					r_type64(ELF64_R_TYPE(rela->r_info)));
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
	int class;

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
	class = file_mmbase[EI_CLASS];

	for (size_t i = 0; i < shnum; i++) {
		ElfW(Shdr) *shdr = &shdrs[i];	

		const char *shname = file_mmbase + shdrs[shstrndx].sh_offset + shdr->sh_name;

		if (shdr->sh_type == SHT_REL || shdr->sh_type == SHT_RELA) {

  			// Get the symbol table information. 
			// for SHT_REL or SHT_RELA:
			// 		sh_link: The section header index of the associated symbol table.
			// 		sh_info: The section header index of the section to which the relocation applies.
			size_t symtabndx = shdr->sh_link;
			ElfW(Sym) *symtab = (ElfW(Sym *))(file_mmbase + shdrs[symtabndx].sh_offset);
			size_t symtab_entries = shdrs[symtabndx].sh_size / shdrs[symtabndx].sh_entsize;
			// for SHT_SYMTAB or SHT_DYNSYM
			// 		sh_info: One greater than the symbol table index of the last local symbol (binding STB_LOCAL).
			// 		sh_link: The section header index of the associated string table.
			size_t strtabndx = shdrs[symtabndx].sh_link;
			const char *strtab = file_mmbase + shdrs[strtabndx].sh_offset; 

			size_t reltab_entries = shdr->sh_size / shdr->sh_entsize;

			(void)printf("\nRelocation section '%s' at offset 0x%jx contains %zu entries:\n",
					shname, (uintmax_t)shdr->sh_offset, reltab_entries);

			if (shdr->sh_type == SHT_REL) {
				ElfW(Rel) *rels = (ElfW(Rel) *)(file_mmbase + shdr->sh_offset);
	//			print_rel();
			}

			if (shdr->sh_type == SHT_RELA) {
				ElfW(Rela) *relas = (ElfW(Rela) *)(file_mmbase + shdr->sh_offset);
				print_rela(relas, reltab_entries, symtab, symtab_entries, strtab, class);	
			}
		}

		

//
//		if (shdr->sh_type == SHT_SYMTAB || shdr->sh_type == SHT_DYNSYM) {
//			const char *shname = file_mmbase + shdrs[shstrndx].sh_offset + shdr->sh_name;
//			ElfW(Sym) *syms = (ElfW(Sym *))(file_mmbase + shdr->sh_offset); 
//			size_t entries = shdr->sh_size / shdr->sh_entsize;
//			// sh_info: One greater than the symbol table index of the last local symbol (binding STB_LOCAL).
//			// printf("shdr->sh_info = %u\n", shdr->sh_info);
//			// sh_link: .strtab or .dynstr (The section header index of the associated string table.)
//			const char *strtab = file_mmbase + shdrs[shdr->sh_link].sh_offset;
//			print_syms(shname, syms, entries, strtab);	
//		}
	}

	(void)munmap(file_mmbase, fsize);
	(void)close(fd);
	
	exit(EXIT_SUCCESS);
}

