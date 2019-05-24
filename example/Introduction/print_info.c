#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include <sys/mman.h>
#include <errno.h>
#include <error.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

static const char *osabi[] = {
	[0] = "UNIX System V ABI",
	[1] = "HP-UX",
	[2] = "NetBSD.",
	[3] = "Object uses GNU ELF extensions.",
	[4] = "",
	[5] = "",
	[6] = "Sun Solaris.",
	[7] = "IBM AIX.",
	[8] = "SGI Irix.",
	[9] = "FreeBSD.",
	[10] = "Compaq TRU64 UNIX.",
	[11] = "Novell Modesto.",
	[12] = "OpenBSD.",
	[13 ... 63] = "",
	[64] = "ARM EABI",
	[65 ... 96] = "",
	[97] = "ARM",
};

int main(int argc, const char *argv[])
{
	int fd;
	char *elf_ident;
	struct stat file_status;
	int i;

	if (argc != 2) {
		error(EXIT_FAILURE, 0, "Usage: %s file-name", argv[0]);
	}

	if ((fd = open(argv[1], O_RDONLY)) < 0) {
		error(EXIT_FAILURE, errno, "open %s failed", argv[1]);
	}

	if (fstat(fd, &file_status) < 0) {
		error(EXIT_FAILURE, errno, "get file %s info err", argv[1]);
	}

	if ((elf_ident = mmap(NULL, (size_t)file_status.st_size,
		PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
		error(EXIT_FAILURE, errno, "mmap file %s err", argv[1]);
	}

	(void)fprintf(stdout, "Magic: ");
	for (i = 0; i < EI_NIDENT; i++) {
		(void)fprintf(stdout, "%02x ", elf_ident[i]);
	}
	(void)fprintf(stdout, "\n");

	if (memcmp(elf_ident, ELFMAG, SELFMAG) != 0) {
		error(EXIT_FAILURE, 0, "Invalid file format");
	}

	(void)fprintf(stdout, "EI_CLASS is %s\n",
		elf_ident[EI_CLASS] == ELFCLASS32 ? "32-bit" : "64-bit");
	(void)fprintf(stdout, "EI_DATA is %s\n",
		elf_ident[EI_DATA] == ELFDATA2LSB ? "little-endian" : "big-endian");
	(void)fprintf(stdout, "EI_VERSION is %d\n", elf_ident[EI_VERSION]);
	(void)fprintf(stdout, "EI_OSABI is %s\n", osabi[(int)elf_ident[EI_OSABI]]);
	(void)fprintf(stdout, "EI_ABIVERSION is %d\n", elf_ident[EI_ABIVERSION]);

	munmap(elf_ident, (size_t)file_status.st_size);
	close(fd);

	exit(EXIT_SUCCESS);
}
