#include "elf_decoder.hpp"

using namespace ELF_DECODER;

/***********          ELF HEADER          ***********/

void ElfHeader::ELF_get_data(uint8_t length, ELF_parts part, uint8_t &dest)
{
	if(!(part == ELF_parts::ELF_Header))
		return;

	/* The method, per this given class, does not read memory into a structure.
	* Always read at least one byte.
	* */
	if(length == 0)
		length = 1;

	/* Read in the data. */
	length = ELF_read_binary(length, &dest, *pholder);
}

ElfHeader::ELF_header &ElfHeader::get_elf_header()
{
    uint8_t *read_in_data = nullptr;

    const auto check_read_in_data = [&read_in_data] (uint8_t new_size)
    {
        if(read_in_data)
            delete read_in_data;
        
        read_in_data = new uint8_t[new_size];
    };

    const auto get_single_byte = [&read_in_data, &check_read_in_data, this] ()
    {
        check_read_in_data(1);

        ELF_get_data(1, ELF_parts::ELF_Header, *read_in_data);
    };

    const auto get_two_bytes = [&read_in_data, &check_read_in_data, this] ()
    {
        check_read_in_data(2);

        ELF_get_data(2, ELF_parts::ELF_Header, *read_in_data);
    };

    const auto get_four_bytes = [&read_in_data, &check_read_in_data, this] ()
    {
        check_read_in_data(4);

        ELF_get_data(4, ELF_parts::ELF_Header, *read_in_data);
    };

    {
        /* Magic Number. */
        get_four_bytes();
        elf_header->ELF_magic = make_into_complete_value<uint32_t> (strlen((const char *)read_in_data), read_in_data);
        check_data<uint32_t> (elf_header->ELF_magic, ELF_MAGIC_NUMBER);
    }

    {
        /* ELF type (32-bit or 64-bit). */
        get_single_byte();
        elf_header->ELF_type = read_in_data[0];
        ELF_ASSERT(elf_header->ELF_type != (uint8_t) ELF_types::Invalid,
           "\nInvalid ELF type. The value at byte 5 should be 0x01 for 32-bit or 0x02 for 64-bit.\n")

        /* ELF endianess. */
        get_single_byte();
        elf_header->ELF_endianess = read_in_data[0];
        ELF_ASSERT(elf_header->ELF_endianess != (uint8_t) ELF_endianess::InvalidE,
            "\nInvalid ELF endianess type. The value at byte 6 should be 0x01 for Little Endian or 0x02 for Big Endian.\n")

        /* ELF version. */
        get_single_byte();
        elf_header->ELF_version = read_in_data[0];
        ELF_ASSERT(elf_header->ELF_version == (uint8_t) ELF_CURRENT_VERSION,
            "\nInvalid ELF version. The value at byte 7 should be 0x01, as that is the only version of ELF supported.\n")

        /* 9 bytes of padding. */
        get_four_bytes();
        get_four_bytes();
        get_single_byte();
    }

    {
        /* ELF file type. */
        get_two_bytes();
        elf_header->ELF_file_type = (make_into_complete_value<uint16_t> (2, read_in_data) >> 8) & 0xFF;

        /* ELF machine type. */
        get_two_bytes();
        elf_header->ELF_machine_type = (make_into_complete_value<uint16_t> (2, read_in_data) >> 8) & 0xFF;
    }

    {
        /* Get the ELF version, again. */
        get_four_bytes();
        elf_header->ELF_version2 = (make_into_complete_value<uint32_t> (4, read_in_data) >> 24) & 0xFF;
        check_data<uint32_t> (elf_header->ELF_version2, (uint32_t) ELF_CURRENT_VERSION);

        /* Get the entry point. */
        get_four_bytes();
        elf_header->ELF_entry = revert_value<uint32_t> (make_into_complete_value<uint32_t> (4, read_in_data));
        
        /* Get the Program Header offset. */
        get_four_bytes();
        elf_header->ELF_PH_offset = revert_value<uint32_t> (make_into_complete_value<uint32_t> (4, read_in_data));

        /* Get the Section Header offset. */
        get_four_bytes();
        elf_header->ELF_SH_offset = revert_value<uint32_t> (make_into_complete_value<uint32_t> (4, read_in_data));

        /* Get ELF flags. */
        get_four_bytes();
        elf_header->ELF_flags = revert_value<uint32_t> (make_into_complete_value<uint32_t> (4, read_in_data));

        delete read_in_data;
    }

    {
        /* Get ELF Header Size (in bytes). */
        get_two_bytes();
        elf_header->ELF_hsize = revert_value<uint16_t> (make_into_complete_value<uint16_t> (2, read_in_data));
        check_data<uint16_t> (elf_header->ELF_hsize, ELF_HEADER_SIZE);

        /* Get Program Header entry size. */
        get_two_bytes();
        elf_header->ELF_PH_entry_size = revert_value<uint16_t> (make_into_complete_value<uint16_t> (2, read_in_data));
        check_data<uint16_t> (elf_header->ELF_PH_entry_size, ELF_PROGRAM_HEADER_SIZE);

        /* Get the amount of Program Header entries. */
        get_two_bytes();
        elf_header->ELF_PH_entry_amnt = revert_value<uint16_t> (make_into_complete_value<uint16_t> (2, read_in_data));

        /* Get the Section Header size. */
        get_two_bytes();
        elf_header->ELF_SH_size = revert_value<uint16_t> (make_into_complete_value<uint16_t> (2, read_in_data));
        check_data<uint16_t> (elf_header->ELF_SH_size, ELF_SECTION_HEADER_SIZE);

        /* Get the amount of Section Header entries. */
        get_two_bytes();
        elf_header->ELF_SH_entry_amnt = revert_value<uint16_t> (make_into_complete_value<uint16_t> (2, read_in_data));

        /* Get the index of the entry associated with the section name string table. */
        get_two_bytes();
        elf_header->ELF_SH_str_index = revert_value<uint16_t> (make_into_complete_value<uint16_t> (2, read_in_data));

        delete read_in_data;
    }

    printf("\nELF Signature:             \t       \e[0;32m%X\e[0;37m\nELF Bit Type:              \t       \e[0;32m0x%X\e[0;37m (\e[0;35m%s\e[0;37m)\nELF Endianess:             \t       \e[0;32m0x%X\e[0;37m (\e[0;35m%s\e[0;37m)\n",
        elf_header->ELF_magic,
        elf_header->ELF_type,
        get_ELF_type_name((ELF_types) elf_header->ELF_type),
        elf_header->ELF_endianess,
        get_ELF_endianess_name((ELF_endianess) elf_header->ELF_endianess));
    
    printf("ELF Version:               \t       \e[0;32m0x%X\e[0;37m\nELF File Type:             \t       \e[0;32m0x%X\e[0;37m (\e[0;35m%s\e[0;37m)\nELF Machine Type:          \t       \e[0;32m0x%X\e[0;37m (\e[0;35m%s\e[0;37m)\n",
        elf_header->ELF_version,
        elf_header->ELF_file_type,
        get_ELF_file_type_name((ELF_file_types) elf_header->ELF_file_type),
        elf_header->ELF_machine_type,
        get_ELF_machine_type_name((ELF_machine_types) elf_header->ELF_machine_type));
    
    printf("ELF Entry:                 \t       \e[0;32m0x%X\e[0;37m\nELF Program Header Offset: \t       \e[0;32m0x%X\e[0;37m\n",
        elf_header->ELF_entry,
        elf_header->ELF_PH_offset);
    
    printf("ELF Section Header Offset: \t       \e[0;32m0x%X\e[0;37m\nELF Flags:                 \t       \e[0;32m0x%X\e[0;37m\nELF Header Size:           \t       \e[0;32m0x%X\e[0;37m (\e[0;35m%d bytes\e[0;37m)\n",
        elf_header->ELF_SH_offset,
        elf_header->ELF_flags,
        elf_header->ELF_hsize,
        elf_header->ELF_hsize);
    
    printf("ELF Program Header Entry Size:         \e[0;32m0x%X\e[0;37m (\e[0;35m%d bytes\e[0;37m)\nELF Program Header Entry Amount:       \e[0;32m0x%X\e[0;37m (\e[0;35m%d entries\e[0;37m)\n",
        elf_header->ELF_PH_entry_size,
        elf_header->ELF_PH_entry_size,
        elf_header->ELF_PH_entry_amnt,
        elf_header->ELF_PH_entry_amnt);
    
    printf("ELF Section Header Size:               \e[0;32m0x%X\e[0;37m (\e[0;35m%d bytes\e[0;37m)\nELF Section Header Entry Amount:       \e[0;32m0x%X\e[0;37m (\e[0;35m%d entries\e[0;37m)\n",
        elf_header->ELF_SH_size,
        elf_header->ELF_SH_size,
        elf_header->ELF_SH_entry_amnt,
        elf_header->ELF_SH_entry_amnt);
    
    printf("ELF Section Header String Table Index: \e[0;32m0x%X\e[0;37m\n\n",
        elf_header->ELF_SH_str_index);

    //ELF_get_data()

    if(read_in_data)
        delete read_in_data;
    read_in_data = nullptr;

    return *elf_header;
}

/***********          END ELF HEADER          ***********/
