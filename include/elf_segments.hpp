#ifndef ELF_SEGMENTS_H
#define ELF_SEGMENTS_H
#include "common.hpp"
#include "elf_sections.hpp"
using namespace elf_sections;

namespace elf_segments
{
    /* Each "segment" has section data. */
	class ElfSegment : private ElfSection
	{
	public:
        ElfSegment() = default;
		ElfSegment(FILE *f, int8_t &filename)
			: ElfSection(f, filename)
		{}

		template<typename T>
			requires std::is_same<T, ElfSegment *>::value
		void delete_instance(T instance)
		{
			if(instance)
				delete instance;
			instance = nullptr;
		}

		~ElfSegment() = default;
	};
}

#endif