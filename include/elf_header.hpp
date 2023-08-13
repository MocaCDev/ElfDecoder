#ifndef ELF_HEADER_H
#define ELF_HEADER_H
#include "common.hpp"

#define ELF_MAGIC_NUMBER        0x7F454C46
#define ELF_CURRENT_VERSION     0x01

/* Structure sizes, per the spec. */
#define ELF_HEADER_SIZE			0x34
#define ELF_PROGRAM_HEADER_SIZE	0x20
#define ELF_SECTION_HEADER_SIZE	0x28

namespace elf_header
{
    enum class ELF_types: uint8_t
	{
		Invalid = 0x0,
		ELF32	= 0x1,
		ELF64	= 0x2
	};

	static uint8_t *get_ELF_type_name(ELF_types elf_type)
	{
		switch(elf_type)
		{
			case ELF_types::ELF32: return (uint8_t *) "32-bit";break;
			case ELF_types::ELF64: return (uint8_t *) "64-bit";break;
			default: break;
		}

		return (uint8_t *) "Unknown Type";
	}

	enum class ELF_endianess: uint8_t
	{
		InvalidE	= 0x0,
		LittleE		= 0x1,
		BigE		= 0x2
	};

	static uint8_t *get_ELF_endianess_name(ELF_endianess endianess)
	{
		switch(endianess)
		{
			case ELF_endianess::LittleE: return (uint8_t *) "Little Endian";break;
			case ELF_endianess::BigE: return (uint8_t *) "Big Endian";break;
			default: break;
		}

		return (uint8_t *) "Unknown Endianess";
	}

	enum class ELF_file_types: uint16_t
	{
		NoFileType	= 0x0,
		RFileType	= 0x1, 		/* Relocatable File */
		EFileType	= 0x2, 		/* Executable File */
		SOFileType	= 0x3, 		/* Shared Object File */
		CFileType	= 0x4, 		/* Core File */
		PSFileType1	= 0xFF00,	/* Processor-specific File (indicator 1) */
		PSFileType2	= 0xFFFF	/* Processor-specific File (indicator 2) */
	};

	static uint8_t *get_ELF_file_type_name(ELF_file_types file_type)
	{
		switch(file_type)
		{
			case ELF_file_types::NoFileType: return (uint8_t *) "No File Type";break;
			case ELF_file_types::RFileType:  return (uint8_t *) "Relocatable File";break;
			case ELF_file_types::EFileType:  return (uint8_t *) "Executable File";break;
			case ELF_file_types::SOFileType: return (uint8_t *) "Shared Object File";break;
			case ELF_file_types::CFileType:  return (uint8_t *) "Core File";break;
			case ELF_file_types::PSFileType1:return (uint8_t *) "Processor-specific File (indicator 1)";break;
			case ELF_file_types::PSFileType2:return (uint8_t *) "Processor-specific File (indicator 2)";break;
			default: break;
		}

		return (uint8_t *) "Unknown File Type";
	}

	enum class ELF_machine_types: uint8_t
	{
		NoMachine	= 0x0,
		ATT_WE_32100	= 0x1,
		SPARC		= 0x2,
		Intel80386	= 0x3,
		Mot68000	= 0x4,		/* Motorola 68000 */
		Mot88000	= 0x5,		/* Motorola 88000 */
		Intel80860	= 0x6,
		MIPS_RS3000	= 0x8
	};

	static uint8_t *get_ELF_machine_type_name(ELF_machine_types machine_type)
	{
		switch(machine_type)
		{
			case ELF_machine_types::NoMachine: return (uint8_t *) "No Machine";break;
			case ELF_machine_types::ATT_WE_32100: return (uint8_t *) "ATT WE 32100";break;
			case ELF_machine_types::SPARC: return (uint8_t *) "SPARC";break;
			case ELF_machine_types::Intel80386: return (uint8_t *) "Intel 80386";break;
			case ELF_machine_types::Mot68000: return (uint8_t *) "Mot 68000";break;
			case ELF_machine_types::Mot88000: return (uint8_t *) "Mot 88000";break;
			case ELF_machine_types::Intel80860: return (uint8_t *) "Intel 80860";break;
			case ELF_machine_types::MIPS_RS3000: return (uint8_t *) "MIPS RS3000";break;
			default: break;
		}

		return (uint8_t *) "Unknown Machine Type";
	}

	/* All possible errors to encounter. */
	enum class ELF_errors
	{
		Invalid_ELF_Header 	= 0x0,
		Invalid_ELF_Section	= 0x1,
		Invalid_ELF_SectionHT	= 0x2,
		NoError			= 0x3
	};

	static uint8_t *get_ELF_error_name(ELF_errors error)
	{
		switch(error)
		{
			case ELF_errors::Invalid_ELF_Header: return (uint8_t *) "Invalid Header";break;
			case ELF_errors::Invalid_ELF_Section: return (uint8_t *) "Invalid Section";break;
			case ELF_errors::Invalid_ELF_SectionHT: return (uint8_t *) "Invalid Section Header Table";break;
			case ELF_errors::NoError: return (uint8_t *) "No Error";break;
			default: break;
		}

		return (uint8_t *) "Unknown Error";
	}

    class ElfHeader
	{
	private:
		/* The first 32-bytes of any ELF binary file will be the following. */
		struct ELF_header
		{
			/* First 16-bytes of the ELF binary. */
			uint32_t	ELF_magic;			/* Must be `ELF_MAGIC_NUMBER`. */
			uint8_t		ELF_type;			/* 0 - Invalid, 1 - 32-bit, 2 - 64-bit. */
			uint8_t		ELF_endianess;		/* 0 - Invalid, 1 - little endian, 2 - big endian. */
			uint8_t		ELF_version;		/* Must be `ELF_CURRENT_VERSION`. */
			uint8_t		ELF_padding[9];

			/* Rest of the header. */
			uint16_t	ELF_file_type;
			uint16_t	ELF_machine_type;
			uint32_t	ELF_version2;		/* Should be same as `ELF_version`. Idk why it repeates. */
			uint32_t	ELF_entry;
			uint32_t	ELF_PH_offset;		/* Program Header offset. */
			uint32_t	ELF_SH_offset;		/* Section Header offset. */
			uint32_t	ELF_flags;		
			uint16_t	ELF_hsize;			/* Header Size, in bytes. */
			uint16_t	ELF_PH_entry_size;	/* 1 Entry size, in bytes, of the Program Header Table. */
			uint16_t	ELF_PH_entry_amnt;	/* Number of entries in the Program Header Table. */
			uint16_t	ELF_SH_size;		/* Section Header size, in bytes. */
			uint16_t	ELF_SH_entry_amnt;	/* Number of entries in the Section Header Table. */
			uint16_t	ELF_SH_str_index;	/* Index of the entry associated with the section name string table. */

			ELF_header() = default;

			template<typename T>
				requires std::is_same<T, struct ELF_header *>::value
					 || std::is_same<T, ElfHeader *>::value
			void delete_instance(T instance)
			{
				if(instance)
					delete instance;
				instance = nullptr;
			}

			~ELF_header() = default;
		};

	protected:
		struct ELF_header *elf_header;
		ElfDecoder *edecoder;
		int8_t &efilename;
	
	public:
		ElfHeader(FILE *f, int8_t &filename)
			: efilename(filename), elf_header(nullptr), edecoder(nullptr)
		{
			elf_header = new struct ELF_header;
			edecoder = new ElfDecoder(f);

			/* Get the ELF header. */
		}

		void ELF_get_data(uint8_t length, ELF_parts part, uint8_t &dest);
		void gather_ELF_heading();
		//struct ELF_header &get_elf_header();
		void get_elf_header();
		void print_elf_header();

		template<typename T>
			requires std::is_same<T, struct ELF_header *>::value
				|| std::is_same<T, ElfHeader *>::value
		void delete_instance(T instance)
		{
			if(instance)
				delete instance;
			instance = nullptr;
		}

		~ElfHeader()
		{
			if(elf_header) delete elf_header;
			elf_header = nullptr;

			delete edecoder;
		}
	};
}

#endif