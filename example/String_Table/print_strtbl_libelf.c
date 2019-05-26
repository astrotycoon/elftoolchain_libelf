#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

static void print_strtbl(Elf_Scn *scn, size_t len)
{
	assert(scn);

	Elf_Data *data = NULL;
	size_t offset = 0;

	for ( ;; ) {
		if ((data = elf_getdata(scn, data)) == NULL) break;
		
		const char *p = data->d_buf;
		while (p < (char *)data->d_buf + data->d_size) {
			(void)printf("  [%6x]  %s\n", (unsigned int)offset, p);	
			offset += strlen(p) + 1;
			p += strlen(p) + 1;
			if (offset >= len) break;
		}
	}
}


int main(int argc, const char *argv[])
{
	int fd, class;
	Elf *pelf = NULL;
	Elf_Scn *scn = NULL;
	GElf_Shdr shdr;
	size_t shstrndx;

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
		if (gelf_getshdr(scn, &shdr) != &shdr) {
			errx(EXIT_FAILURE, "gelf_getshdr() failed: %s.", elf_errmsg(-1));	
		}

		if (shdr.sh_type != SHT_STRTAB) continue;

		(void)printf("String dump of section '%s':\n", 
				elf_strptr(pelf, shstrndx, shdr.sh_name));
		print_strtbl(scn, shdr.sh_size);
		printf("\n");
	}	

	(void)elf_end(pelf);
	(void)close(fd);

	exit(EXIT_SUCCESS);
}
