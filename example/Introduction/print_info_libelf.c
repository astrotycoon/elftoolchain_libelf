#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include "libelf.h"
#include "gelf.h"

int main(int argc, const char *argv[])
{
	int i, fd;
	Elf *pelf = NULL;
	char *elf_ident;

	if (argc != 2)
		errx(EXIT_FAILURE, "usage: %s file-name", argv[0]);

	if (elf_version(EV_CURRENT) == EV_NONE)
		errx(EXIT_FAILURE, "ELF library initializztion "
			"failed: %s", elf_errmsg(-1));

	if ((fd = open(argv[1], O_RDONLY | O_NONBLOCK, 0)) < 0)
		errx(EXIT_FAILURE, "open \"%s\" failed", argv[1]);

	if (!(pelf = elf_begin(fd, ELF_C_READ, NULL)))
		errx(EXIT_FAILURE, "elf_begin() failed: %s", elf_errmsg(-1));

	if (elf_kind(pelf) != ELF_K_ELF)
		errx(EXIT_FAILURE, "\"%s\" is not an ELF object.", argv[1]);

	// get elf class ()
	if ((i = gelf_getclass(pelf)) == ELFCLASSNONE)
		errx(EXIT_FAILURE, "getclass() failed: %s.", elf_errmsg(-1));
	// print the elf class
	printf("%s: %d-bit ELF object\n", argv[1],
		(i == ELFCLASS32) ? 32 : 64);
	// get e_elf_identent
	if ((elf_ident = elf_getident(pelf, NULL)) == NULL)
		errx(EXIT_FAILURE, "getelf_identent() failed: %s.", elf_errmsg(-1));
	// print e_elf_identent
	printf("Magic: ");
	for (i = 0; i <= EI_ABIVERSION; i++) {
		printf("0x%02x ", elf_ident[i]);
	}
	printf("\n");
	for (i = 0; i <= EI_ABIVERSION; i++) {
		switch (i) {
		case 0:
			printf("0x%02x ", elf_ident[i]);
			break;
		case 1 ... 3:
			printf("%c ", elf_ident[i]);
			break;
		case 4:
			printf("%d-bit ", elf_ident[i] == ELFCLASS32 ? 32 : 64);
			break;
		case 5:
			printf("%s ", elf_ident[i] == ELFDATA2LSB ? "LSB" : "MSB");
			break;
		case 6:
			printf("%s ", elf_ident[i] == EV_CURRENT ? "EV_CURRENT" : "EV_NONE");
			break;
		case 7:
			printf("%s ", elf_ident[i] == ELFOSABI_NONE ? "ELFOSABI_NONE" : " ");
			break;
		case 8:
			printf("%d ", 0);
			break;
		}
	}
	printf("\n");

	elf_end(pelf);
	exit(EXIT_SUCCESS);
}
