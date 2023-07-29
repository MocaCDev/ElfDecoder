#ifndef ELF_DECODER_H
#define ELF_DECODER_H
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <stdint.h>

#define ELF_MAGIC_NUMBER        0x7F454C46
#define ELF_CURRENT_VERSION     0x01

#ifdef NULL
#undef NULL
#endif

#define NULL			(uint8_t) 0

#define ELF_ASSERT(cond, msg, ...)              \
if(!(cond))					\
{						\
	fprintf(stderr, msg, ##__VA_ARGS__);	\
	exit(EXIT_FAILURE);			\
}

namespace ELF_DECODER
{
	enum class ELF_types: uint8_t
	{
		Invalid = 0x0,
		ELF32	= 0x1,
		ELF64	= 0x2
	};

	enum class ELF_endianess: uint8_t
	{
		InvalidE	= 0x0,
		LittleE		= 0x1,
		BigE		= 0x2
	};

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

	/* Parts of the ELF binary. */
	enum class ELF_parts: uint8_t
	{
		ELF_Header	= 0x0,
		ELF_SectionD	= 0x1, 		/* Section Data - part of segments */
		ELF_SectionHT	= 0x2,		/* Section Header Table */
	};

	/* All possible errors to encounter. */
	enum class ELF_errors
	{
		Invalid_ELF_Header 	= 0x0,
		Invalid_ELF_Section	= 0x1,
		Invalid_ELF_SectionHT	= 0x2,
		NoError			= 0x3
	};

	/* Common functionality to be found in each "step" of decoding the ELF binary file. */
	class ElfDecoder
	{
	protected:
		FILE *ELF_binary;
		size_t seek_pos;
		size_t backup_seek_pos;

		void ELF_bin_file_is_valid()
		{
			ELF_ASSERT(ELF_binary,
				"\nThe ELF binary closed unexpectedly, or an unknown error ocurred.\n")
		}

		/* Only used for `ELF_read_binary`. */
		struct placeholder
		{
			placeholder() = default;

			~placeholder() = default;
		};
		static placeholder pholder;

		template<typename T = placeholder>
			requires std::is_class<T>::value
		uint8_t ELF_read_binary(uint8_t &bytes, uint8_t *data, T structure = pholder)
		{
			/* Will only read, max, 4 bytes at a time. */
			if(bytes > 4)
				bytes = 4;
			
			/* Keep track of how many bytes we are supposed to read in. */
			uint8_t bytes_to_read = bytes;

			if(bytes == NULL)
				ELF_ASSERT(!(std::is_same<T, placeholder>::value),
					"\nCannot read memory into a nullptr.\n")

			/* Make sure `data` is not `nullptr`. */
			ELF_ASSERT(data != nullptr,
				"\nCannot read memory into a nullptr.\n")

			/* Zero out all memory. */
			memset(data, 0, bytes);

			/* Save the "last state" of the previous seek position. */
			backup_seek_pos = seek_pos;

			while(bytes > 0)
			{
				ELF_ASSERT(fread(&data[bytes], sizeof(uint8_t), 1, ELF_binary) == 1,
					"\nThere was an error reading data from the ELF binary file.\n")
				
				seek_pos++;
				bytes--;
			}
			
			/* Make sure we read in the correct amount. */
			ELF_ASSERT(seek_pos - backup_seek_pos == bytes_to_read,
				"\nA very unknown error ocurred where the seek position did not increment by %d bytes.\n",
				bytes)
			return bytes_to_read;
		}

		template<typename T>
			requires (std::is_same<T, uint16_t>::value
				|| std::is_same<T, uint32_t>::value)
				&& (!std::is_same<T, uint8_t>::value)
		T make_into_complete_value(uint8_t bytes, uint8_t *data)
		{
			T complete_value = 0;
			
			complete_value |= data[sizeof(T)] & 0xFF;
			complete_value = (complete_value << 8) | (data[sizeof(T) - 1] & 0xFF);

			uint8_t bytes_in = 2;
			reloop:
			if(bytes_in >= sizeof(T)) goto end;

			/* This should always run with `uint32_t`. This function doesn't allow
			 * `uint64_t`, which is why the code below it should be removed.
			 * TODO: I am lazy right now, remove the code below the if statement, thereafter remove the if statement as well and just keep
			 * 	 the code that resides inside of it.
			 * */
			if((bytes_in + 2) >= sizeof(T))
			{
				complete_value = (complete_value << 8) | (data[2] & 0xFF);
				complete_value = (complete_value << 8) | (data[1] & 0xFF);
				goto end;
			}

			/* We do not need this, however I am going to keep it just to make sure. */
			complete_value = (complete_value << 8) | (data[sizeof(T) - bytes_in] & 0xFF);
			complete_value = (complete_value << 8) | (data[sizeof(T) - (bytes_in - 1)] & 0xFF);
			bytes_in += 2;
			goto reloop;

			end:
			return (T) complete_value;
		}


	public:
		ElfDecoder(FILE *f)
			: ELF_binary(f), seek_pos(0), backup_seek_pos(0)
		{
			ELF_ASSERT(ELF_binary,
				"\nThe ELF binary file does not exist.\n")
		}

		virtual uint8_t *ELF_get_data(uint8_t length, ELF_parts part) = 0;
		virtual ELF_errors check_data(uint8_t *data) = 0;

		~ElfDecoder()
		{
			if(ELF_binary) fclose(ELF_binary);
			ELF_binary = nullptr;
		}
	};

	class ElfHeader : public ElfDecoder
	{
	private:
		#pragma pack(4)
		/* The first 32-bytes of any ELF binary file will be the following. */
		struct ELF_header
		{
			/* First 16-bytes of the ELF binary. */
			uint32_t	ELF_magic;		/* Must be `ELF_MAGIC_NUMBER`. */
			uint8_t		ELF_type;		/* 0 - Invalid, 1 - 32-bit, 2 - 64-bit. */
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
			uint16_t	ELF_hsize;		/* Header Size, in bytes. */
			uint16_t	ELF_PH_entry_size;	/* 1 Entry size, in bytes, of the Program Header Table. */
			uint16_t	ELF_PH_entry_amnt;	/* Number of entries in the Program Header Table. */
			uint16_t	ELF_SH_size;		/* Section Header size, in bytes. */
			uint16_t	ELF_SH_entry_amnt;	/* Number of entries in the Section Header Table. */
			uint16_t	ELF_SH_str_index;	/* Index of the entry associated with the section name string table. */

			ELF_header() = default;

			template<typename T>
				requires std::is_same<T, struct ELF_header>::value
					 || std::is_same<T, ElfHeader>::value
			void delete_instance(T instance)
			{
				if(instance)
					delete instance;
				instance = nullptr;
			}

			~ELF_header() = default;
		};
		#pragma pop

	protected:
		struct ELF_header *elf_header;
	
	public:
		ElfHeader(FILE *f)
			: elf_header(nullptr), ElfDecoder(f)
		{
			elf_header = new struct ELF_header;
		}

		uint8_t *ELF_get_data(uint8_t length, ELF_parts part) override
		{
			if(!(part == ELF_parts::ELF_Header))
				return nullptr;

			/* The method, per this given class, does not read memory into a structure.
			 * Always read at least one byte.
			 * */
			if(length == 0)
				length = 1;
			
			/* Make sure the file is valid. */
			ELF_bin_file_is_valid();

			/* Read in the data. */
			uint8_t *data = new uint8_t[length];
			length = ELF_read_binary(length, data);
			
			printf("%X\n", make_into_complete_value<uint32_t> (length, data));
			exit(EXIT_SUCCESS);

			return (uint8_t *) 0;
		}

		ELF_errors check_data(uint8_t *data) override
		{
			return ELF_errors::NoError;
		}

		template<typename T>
			requires std::is_same<T, struct ELF_header>::value
				|| std::is_same<T, ElfHeader>::value
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
		}
	};

	/* Section data that belongs to each segment found in the ELF binary. */
	class ElfSection : public ElfHeader
	{
	public:
		ElfSection(FILE *f)
			: ElfHeader(f)
		{}

		~ElfSection() = default;
	};

	/* Each "segment" has section data. */
	class ElfSegment : private ElfSection
	{
	public:
		ElfSegment(FILE *f)
			: ElfSection(f)
		{}

		~ElfSegment() = default;
	};

	/* Entirety of the ELF binary. */
	class ElfBinary : private ElfSegment
	{
	public:
		ElfBinary(FILE *f)
			: ElfSegment(f)
		{}

		~ElfBinary() = default;
	};
}

#endif
