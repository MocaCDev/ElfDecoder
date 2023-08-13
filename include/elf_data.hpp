#ifndef ELF_DATA_H
#define ELF_DATA_H
#include "common.hpp"
#include "elf_segments.hpp"
using namespace elf_segments;

namespace elf_binary_data
{
    /* Entirety of the ELF binary. */
	class ElfBinary : private ElfSegment
	{
	public:
		ElfBinary() = default;
		ElfBinary(FILE *f, int8_t &filename)
			: ElfSegment(f, filename)
		{}

		template<typename T>
			requires std::is_same<T, ElfBinary *>::value
		void delete_instance(T instance)
		{
			if(instance)
				delete instance;
			instance = nullptr;
		}

		~ElfBinary() = default;
	};
}

#endif