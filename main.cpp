#include "elf_decoder.hpp"

using namespace ELF_DECODER;

int main(int args, char *argv[])
{
	FILE *f = fopen("main.o", "rb");
	
	ElfHeader *eheader = new ElfHeader(f);

	eheader->get_elf_header();

	delete eheader;

	return 0;
}
