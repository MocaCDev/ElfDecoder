#include "elf_decoder.hpp"

using namespace ELF_DECODER;

int main(int args, char *argv[])
{
	FILE *f = fopen("main.o", "rb");
	
	ElfHeader *eheader = new ElfHeader(f);

	eheader->ELF_get_data(4, ELF_parts::ELF_Header);

	delete eheader;

	return 0;
}
