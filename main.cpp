#include "include/elf_decoder.hpp"
#include <vector>
//using namespace elf_header;

int main(int args, char *argv[])
{
	ELF_ASSERT(args > 1,
		"\nExpected ELF binary file as an argument.\n")
	
	FILE *elf_file = nullptr;
	ElfProgramHeader *pheader = nullptr;

	if(strcmp(argv[1], "-f") == 0)
	{
		uint32_t i = 2;
		while(i < args)
		{
			elf_file = fopen(argv[i], "rb");

			/* Decode. */
			pheader = new ElfProgramHeader(elf_file, *(int8_t *)argv[i]);
			pheader->get_program_header_table();

			fclose(elf_file);
			delete pheader;
			i++;
		}

		goto end;
	}

	elf_file = fopen(argv[1], "rb");
	
	pheader = new ElfProgramHeader(elf_file, *(int8_t *)argv[1]);
	pheader->get_program_header_table();

	//delete pheader;

	end:
	return 0;
}
