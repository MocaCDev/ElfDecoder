#ifndef ELF_SECTIONS_H
#define ELF_SECTIONS_H
#include "common.hpp"
#include "elf_program_header.hpp"

using namespace elf_program_header;

namespace elf_sections
{
    /* Section data that belongs to each segment found in the ELF binary. */
	class ElfSection : public ElfProgramHeader
	{
	public:
        ElfSection() = default;
		ElfSection(FILE *f, int8_t &filename)
			: ElfProgramHeader(f, filename)
		{
			get_program_header_table();
			print_elf_program_header_table();
		}

		template<typename T>
			requires std::is_same<T, ElfSection *>::value
		void delete_instance(T instance)
		{
			if(instance)
				delete instance;
			instance = nullptr;
		}

		~ElfSection() = default;
	};
}

#endif