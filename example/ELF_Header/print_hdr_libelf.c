#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <stdint.h>
#include <unistd.h>

static void print_ehdr(Elf *pelf, GElf_Ehdr *ehdr)
{
#define PRINT_FMT	"%-37s%#jx\n"	
#define PRINT_HDR(field) do { (void)printf(PRINT_FMT, #field, (uintmax_t)ehdr->field); } while (0); 

	for (int i = 0; i < EI_NIDENT; i++) {
		printf("%02hhx ", ehdr->e_ident[i]);
	}
	putchar('\n');

	PRINT_HDR(e_type);
	PRINT_HDR(e_machine);
	PRINT_HDR(e_version);
	PRINT_HDR(e_entry);
	PRINT_HDR(e_phoff);
	PRINT_HDR(e_shoff);
	PRINT_HDR(e_flags);
	PRINT_HDR(e_ehsize);
	PRINT_HDR(e_phentsize);
	//	PRINT_HDR(e_phnum);
	PRINT_HDR(e_shentsize);
	//	PRINT_HDR(e_shnum);
	//	PRINT_HDR(e_shstrndx);

	size_t phnum, shnum, shstrndx;

	if (elf_getphdrnum(pelf, &phnum) == -1) {
		(void)fprintf(stderr, "elf_getphdrnum() failed: %s.", elf_errmsg(-1));
	}
	else {
		printf(PRINT_FMT, "(e_phnum)", (uintmax_t)phnum);
	}

	if (elf_getshdrnum(pelf, &shnum) == -1) {
		(void)fprintf(stderr, "elf_getshdrnum() failed: %s.", elf_errmsg(-1));
	}
	else {
		printf(PRINT_FMT, "(e_shnum)", (uintmax_t)shnum);
	}

	if (elf_getshdrstrndx(pelf, &shstrndx) == -1) {
		(void)fprintf(stderr, "elf_getshdrstrndx() failed: %s.", elf_errmsg(-1));
	}
	else {
		printf(PRINT_FMT, "(e_shstrndx)", (uintmax_t)shstrndx);
	}

#undef PRINT_FMT
#undef PRINT_HDR 
}

int main(int argc, const char *argv[])
{
	int fd, class;
	Elf *pelf = NULL;
	GElf_Ehdr ehdr;

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

	// get the elf header
	if (gelf_getehdr(pelf, &ehdr) == NULL)
		errx(EXIT_FAILURE, "getehdr() failed: %s.", elf_errmsg(-1));

	// get elf class ()
	if ((class = gelf_getclass(pelf)) == ELFCLASSNONE)
		errx(EXIT_FAILURE, "getclass() failed: %s.", elf_errmsg(-1));
	// print the elf class
	printf("%s: %d-bit ELF object\n", argv[1],
		(class == ELFCLASS32) ? 32 : 64);

	// print header
	print_ehdr(pelf, &ehdr);

	elf_end(pelf);

	exit(EXIT_SUCCESS);
}
