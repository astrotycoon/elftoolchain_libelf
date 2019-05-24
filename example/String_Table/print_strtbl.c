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
#include <assert.h>

static void print_strtbl(const char *start, size_t len)
{
	assert(start);

	const char *p = start;
	size_t offset = 0;

	for ( ;; ) {
		(void)printf("  [%6x]  %s\n", (unsigned int)offset, p + offset);	
		offset += strlen(p + offset) + 1;
		if (offset >= len) break;
	}
}
 
int main(int argc,	 const char *argv[])
{
	int fd;
	char *file_mmbase;
	struct stat file_status;
	size_t fsize;
	ElfW(Ehdr) *ehdr;
	ElfW(Shdr) *shdrs;
	size_t shnum, shstrndx;
 
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

	for (size_t i = 0; i < shnum; i++) {
		ElfW(Shdr) *shdr = &shdrs[i];	
		if (shdr->sh_type != SHT_STRTAB) continue;

		(void)printf("String dump of section '%s':\n", file_mmbase + shdrs[shstrndx].sh_offset + shdr->sh_name);
		print_strtbl(file_mmbase + shdrs[i].sh_offset, shdrs[i].sh_size);
		(void)printf("\n");
	}
	
	(void)munmap(file_mmbase, fsize);
	(void)close(fd);
	
	exit(EXIT_SUCCESS);
}

