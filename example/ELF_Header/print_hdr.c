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
#include <inttypes.h>

static const char *elf_osabi(uint32_t osabi, char *buf)
{
	assert(buf);

	switch (osabi) {
		//		case ELFOSABI_NONE:
	case ELFOSABI_SYSV:
		(void)strcpy(buf, "UNIX - System V"); break;
	case ELFOSABI_HPUX:
		(void)strcpy(buf, "HP/UX"); break;
	case ELFOSABI_NETBSD:
		(void)strcpy(buf, "NetBSD"); break;
		//		case ELFOSABI_GNU:
		//			(void)strcpy(buf, "GNU"); break;
	case ELFOSABI_LINUX:
		(void)strcpy(buf, "Linux"); break;
	case ELFOSABI_SOLARIS:
		(void)strcpy(buf, "Solaris"); break;
	case ELFOSABI_AIX:
		(void)strcpy(buf, "AIX"); break;
	case ELFOSABI_IRIX:
		(void)strcpy(buf, "Irix"); break;
	case ELFOSABI_FREEBSD:
		(void)strcpy(buf, "FreeBSD"); break;
	case ELFOSABI_TRU64:
		(void)strcpy(buf, "TRU64"); break;
	case ELFOSABI_MODESTO:
		(void)strcpy(buf, "Modesto"); break;
	case ELFOSABI_OPENBSD:
		(void)strcpy(buf, "OpenBSD"); break;
	case ELFOSABI_ARM_AEABI:
		(void)strcpy(buf, "ARM EABIARM"); break;
	case ELFOSABI_ARM:
		(void)strcpy(buf, "ARM"); break;
	case ELFOSABI_STANDALONE:
		(void)strcpy(buf, "Stand alone"); break;
	default:
		(void)sprintf(buf, "%s: %d", "<unknown>", osabi);
		break;
	}

	return (buf);
}

static const char *elf_type(uint32_t type, char *buf)
{
	assert(buf);

	switch (type) {
	case ET_NONE:
		(void)strcpy(buf, "NONE (None)");	break;
	case ET_REL:
		(void)strcpy(buf, "REL (Relocatable file)");	break;
	case ET_EXEC:
		(void)strcpy(buf, "EXEC (Executable file)");	break;
	case ET_DYN:
		(void)strcpy(buf, "DYN (Shared object file)");	break;
	case ET_CORE:
		(void)strcpy(buf, "CORE (Core file)");	break;
	default:
		if (type >= ET_LOOS && type <= ET_HIOS) {
			(void)sprintf(buf, "<os: %#x>", type);
		}
		else if (type >= ET_LOPROC) {
			(void)sprintf(buf, "<proc: %#x>", type);
		}
		else {
			(void)sprintf(buf, "<unknown: %#x>", type);
		}
		break;
	}

	return (buf);
}

static const char *elf_machine(uint32_t machine, char *buf)
{
	assert(buf);

	switch (machine) {
	case EM_NONE: return "Unknown machine";
	case EM_M32: return "AT&T WE32100";
	case EM_SPARC: return "Sun SPARC";
	case EM_386: return "Intel 80386";
	case EM_68K: return "Motorola 68000";
	case EM_88K: return "Motorola 88000";
	case EM_860: return "Intel i860";
	case EM_MIPS: return "MIPS R3000 Big-Endian only";
	case EM_S370: return "IBM System/370";
	case EM_MIPS_RS3_LE: return "MIPS R3000 Little-Endian";
	case EM_PARISC: return "HP PA-RISC";
	case EM_VPP500: return "Fujitsu VPP500";
	case EM_SPARC32PLUS: return "SPARC v8plus";
	case EM_960: return "Intel 80960";
	case EM_PPC: return "PowerPC 32-bit";
	case EM_PPC64: return "PowerPC 64-bit";
	case EM_S390: return "IBM System/390";
	case EM_V800: return "NEC V800";
	case EM_FR20: return "Fujitsu FR20";
	case EM_RH32: return "TRW RH-32";
	case EM_RCE: return "Motorola RCE";
	case EM_ARM: return "ARM";
	case EM_SH: return "Hitachi SH";
	case EM_SPARCV9: return "SPARC v9 64-bit";
	case EM_TRICORE: return "Siemens TriCore embedded processor";
	case EM_ARC: return "Argonaut RISC Core";
	case EM_H8_300: return "Hitachi H8/300";
	case EM_H8_300H: return "Hitachi H8/300H";
	case EM_H8S: return "Hitachi H8S";
	case EM_H8_500: return "Hitachi H8/500";
	case EM_IA_64: return "Intel IA-64 Processor";
	case EM_MIPS_X: return "Stanford MIPS-X";
	case EM_COLDFIRE: return "Motorola ColdFire";
	case EM_68HC12: return "Motorola M68HC12";
	case EM_MMA: return "Fujitsu MMA";
	case EM_PCP: return "Siemens PCP";
	case EM_NCPU: return "Sony nCPU";
	case EM_NDR1: return "Denso NDR1 microprocessor";
	case EM_STARCORE: return "Motorola Star*Core processor";
	case EM_ME16: return "Toyota ME16 processor";
	case EM_ST100: return "STMicroelectronics ST100 processor";
	case EM_TINYJ: return "Advanced Logic Corp. TinyJ processor";
	case EM_X86_64: return "Advanced Micro Devices x86-64";
	case EM_PDSP: return "Sony DSP Processor";
	case EM_FX66: return "Siemens FX66 microcontroller";
	case EM_ST9PLUS: return "STMicroelectronics ST9+ 8/16 microcontroller";
	case EM_ST7: return "STmicroelectronics ST7 8-bit microcontroller";
	case EM_68HC16: return "Motorola MC68HC16 microcontroller";
	case EM_68HC11: return "Motorola MC68HC11 microcontroller";
	case EM_68HC08: return "Motorola MC68HC08 microcontroller";
	case EM_68HC05: return "Motorola MC68HC05 microcontroller";
	case EM_SVX: return "Silicon Graphics SVx";
	case EM_ST19: return "STMicroelectronics ST19 8-bit mc";
	case EM_VAX: return "Digital VAX";
	case EM_CRIS: return "Axis Communications 32-bit embedded processor";
	case EM_JAVELIN: return "Infineon Tech. 32bit embedded processor";
	case EM_FIREPATH: return "Element 14 64-bit DSP Processor";
	case EM_ZSP: return "LSI Logic 16-bit DSP Processor";
	case EM_MMIX: return "Donald Knuth's educational 64-bit proc";
	case EM_HUANY: return "Harvard University MI object files";
	case EM_PRISM: return "SiTera Prism";
	case EM_AVR: return "Atmel AVR 8-bit microcontroller";
	case EM_FR30: return "Fujitsu FR30";
	case EM_D10V: return "Mitsubishi D10V";
	case EM_D30V: return "Mitsubishi D30V";
	case EM_V850: return "NEC v850";
	case EM_M32R: return "Mitsubishi M32R";
	case EM_MN10300: return "Matsushita MN10300";
	case EM_MN10200: return "Matsushita MN10200";
	case EM_PJ: return "picoJava";
	case EM_OPENRISC: return "OpenRISC 32-bit embedded processor";
	case EM_ARC_A5: return "ARC Cores Tangent-A5";
	case EM_XTENSA: return "Tensilica Xtensa Architecture";
	case EM_AARCH64: return "AArch64";
	default:
		(void)sprintf(buf, "<unknown: %#x>", machine);
		break;
	}

	return (buf);
}

static const char *elf_flags(uint32_t machine, uint32_t flags, char *buf)
{
	assert(buf);

	switch (machine) {
	case EM_386:
	case EM_X86_64:
		(void)strcpy(buf, "");
	case EM_ARM:
		/* 平台相关的东西，这里略之 */
		(void)strcpy(buf, "");
		break;
	default:
		/* ... */
		break;
	}

	return (buf);
}

static void print_ehdr(ElfW(Ehdr) *ehdr, ElfW(Shdr) *shdr)
{
	ElfW(Half) phnum, shnum, shstrndx;
	char buf[512];

	phnum = ehdr->e_phnum;
	shnum = ehdr->e_shnum;
	shstrndx = ehdr->e_shstrndx;

	/* e_ident */
	printf("ELF Header:\n  Magic:   ");
	for (int i = 0; i < EI_NIDENT; i++) {
		printf("%02hhx ", ehdr->e_ident[i]);
	}
	printf("\n");

	printf("%-37s%s\n", "  Class:",
		ehdr->e_ident[EI_CLASS] == ELFCLASS32 ? "ELF32"
		: ehdr->e_ident[EI_CLASS] == ELFCLASS64 ? "ELF64"
		: "\?\?\?");
	printf("%-37s%s\n", "  Data:",
		ehdr->e_ident[EI_DATA] == ELFDATA2LSB ? "2's complement, little endian"
		: ehdr->e_ident[EI_DATA] == ELFDATA2MSB ? "2's complement, big endian"
		: "\?\?\?");
	printf("%-37s%hhd (%s)\n", "  Version:", ehdr->e_ident[EI_VERSION],
		ehdr->e_ident[EI_VERSION] == EV_CURRENT ? "current" : "\?\?\?");
	printf("%-37s%s\n", "  OS/ABI:", elf_osabi(ehdr->e_ident[EI_OSABI], buf));
	printf("%-37s%hhd\n", "  ABI Version:", ehdr->e_ident[EI_ABIVERSION]);

	/* e_type */
	printf("%-37s%s\n", "  Type:", elf_type(ehdr->e_type, buf));
	/* e_machine */
	printf("%-37s%s\n", "  Machine:", elf_machine(ehdr->e_machine, buf));
	/* e_version */
	printf("%-37s%#x\n", "  Version:", ehdr->e_version);
	/* e_entry */
//	printf("%-37s%#" PRIxPTR "\n", "  Entry point address:", (uintptr_t)ehdr->e_entry);
	printf("%-37s%#jx\n", "  Entry point address:", (uintmax_t)ehdr->e_entry);

	/* e_phoff */
//	printf("%-37s%#" PRId64 " (bytes into file)\n", 
//			"  Start of program headers:", ehdr->e_phoff);
	printf("%-37s%ju (bytes into file)\n",
		"  Start of program headers:", (uintmax_t)ehdr->e_phoff);
	/* e_shoff */
	printf("%-37s%ju (bytes into file)\n",
		"  Start of section headers:", (uintmax_t)ehdr->e_shoff);

	/* e_flags */
	printf("%-37s%#x, %s\n", "  Flags:",
		ehdr->e_flags, elf_flags(ehdr->e_machine, ehdr->e_flags, buf));
	/* e_ehsize */
	printf("%-37s%u (bytes)\n", "  Size of this header:", ehdr->e_ehsize);

	/* e_phentsize */
	printf("%-37s%u (bytes)\n", "  Size of program headers:", ehdr->e_phentsize);
	/* e_phnum */
	printf("%-37s%u", "  Number of program headers:", phnum);
	if (phnum == PN_XNUM) {
		printf(" (%u)\n", shdr[0].sh_info);
	}

	/* e_shentsize */
	printf("%-37s%u (bytes)\n", "  Size of section headers:", ehdr->e_shentsize);
	/* e_shnum */
	printf("%-37s%u\n", "  Number of section headers:", shnum);
	if (shnum == SHN_UNDEF) {
		printf(" (%ju)\n", (uintmax_t)shdr[0].sh_size);
	}

	/* e_shstrndx */
	printf("%-37s%u\n", "  Section header string table index:", shstrndx);
	if (shstrndx == SHN_XINDEX) {
		printf(" (%u)\n", shdr[0].sh_link);
	}
}

int main(int argc, const char *argv[])
{
	int fd;
	char *file_mmbase;
	struct stat file_status;
	size_t fsize;
	ElfW(Ehdr) *ehdr;
	ElfW(Shdr) *shdr;

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
	shdr = (ElfW(Shdr) *)(file_mmbase + ehdr->e_shoff);
	print_ehdr(ehdr, shdr);

	(void)munmap(file_mmbase, fsize);
	(void)close(fd);

	exit(EXIT_SUCCESS);
}
