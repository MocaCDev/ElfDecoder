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

    {
        /* Magic Number. */
        uint8_t *read_in_magic_number = new uint8_t[4];

        ELF_get_data(4, ELF_parts::ELF_Header, *read_in_magic_number);
        elf_header->ELF_magic = make_into_complete_value<uint32_t> (strlen((const char *)read_in_magic_number), read_in_magic_number);
        check_data<uint32_t> (elf_header->ELF_magic, ELF_MAGIC_NUMBER);

        delete read_in_magic_number;
    }

    {
        /* ELF type (32-bit or 64-bit). */
        uint8_t *read_in_elf_type = new uint8_t[1];

        ELF_get_data(1, ELF_parts::ELF_Header, *read_in_elf_type);
        elf_header->ELF_type = read_in_elf_type[0];
        ELF_ASSERT(elf_header->ELF_type != (uint8_t) ELF_types::Invalid,
           "\nInvalid ELF type. The value at byte 5 should be 0x01 for 32-bit or 0x02 for 64-bit.\n")

        delete read_in_elf_type;
    }

    {
        /* ELF endianess. */
        uint8_t *read_in_elf_endianess = new uint8_t[1];

        ELF_get_data(1, ELF_parts::ELF_Header, *read_in_elf_endianess);
        elf_header->ELF_endianess = read_in_elf_endianess[0];
        ELF_ASSERT(elf_header->ELF_endianess != (uint8_t) ELF_endianess::InvalidE,
            "\nInvalid ELF endianess type. The value at byte 6 should be 0x01 for Little Endian or 0x02 for Big Endian.\n")
        
        delete read_in_elf_endianess;
    }

    {
        /* ELF version. */
        uint8_t *read_in_elf_version = new uint8_t[1];

        ELF_get_data(1, ELF_parts::ELF_Header, *read_in_elf_version);
        elf_header->ELF_version = read_in_elf_version[0];
        ELF_ASSERT(elf_header->ELF_version == (uint8_t) ELF_CURRENT_VERSION,
            "\nInvalid ELF version. The value at byte 7 should be 0x01, as that is the only version of ELF supported.\n")
        
        delete read_in_elf_version;

        /* 9 bytes of padding. */
        read_in_elf_version = new uint8_t[9];
        ELF_get_data(4, ELF_parts::ELF_Header, *read_in_elf_version);
        ELF_get_data(4, ELF_parts::ELF_Header, *read_in_elf_version);
        ELF_get_data(1, ELF_parts::ELF_Header, *read_in_elf_version);
        delete read_in_elf_version;
    }

    {
        /* ELF file type. */
        uint8_t *read_in_data = new uint8_t[2];

        ELF_get_data(2, ELF_parts::ELF_Header, *read_in_data);
        elf_header->ELF_file_type = (make_into_complete_value<uint16_t> (2, read_in_data) >> 8) & 0xFF;

        /* ELF machine type. */
        ELF_get_data(2, ELF_parts::ELF_Header, *read_in_data);
        elf_header->ELF_machine_type = (make_into_complete_value<uint16_t> (2, read_in_data) >> 8) & 0xFF;

        delete read_in_data;
    }

    {
        /* Get the ELF version, again. */
        uint8_t *read_in_data = new uint8_t[4];

        ELF_get_data(4, ELF_parts::ELF_Header, *read_in_data);
        elf_header->ELF_version2 = (make_into_complete_value<uint32_t> (4, read_in_data) >> 24) & 0xFF;
        check_data<uint32_t> (elf_header->ELF_version2, (uint32_t) ELF_CURRENT_VERSION);

        /* Get the entry point. */
        ELF_get_data(4, ELF_parts::ELF_Header, *read_in_data);
        elf_header->ELF_entry = revert_value<uint32_t> (make_into_complete_value<uint32_t> (4, read_in_data));
        
        /* Get the Program Header offset. */
        ELF_get_data(4, ELF_parts::ELF_Header, *read_in_data);
        elf_header->ELF_PH_offset = revert_value<uint32_t> (make_into_complete_value<uint32_t> (4, read_in_data));

        /* Get the Section Header offset. */
        ELF_get_data(4, ELF_parts::ELF_Header, *read_in_data);
        elf_header->ELF_SH_offset = revert_value<uint32_t> (make_into_complete_value<uint32_t> (4, read_in_data));

        /* Get ELF flags. */
        ELF_get_data(4, ELF_parts::ELF_Header, *read_in_data);
        elf_header->ELF_flags = revert_value<uint32_t> (make_into_complete_value<uint32_t> (4, read_in_data));

        delete read_in_data;
    }

    {
        uint8_t *read_in_data = new uint8_t[2];

        /* Get ELF Header Size (in bytes). */
        ELF_get_data(2, ELF_parts::ELF_Header, *read_in_data);
        elf_header->ELF_hsize = revert_value<uint16_t> (make_into_complete_value<uint16_t> (2, read_in_data));
        check_data<uint16_t> (elf_header->ELF_hsize, ELF_HEADER_SIZE);

        /* Get Program Header entry size. */
        ELF_get_data(2, ELF_parts::ELF_Header, *read_in_data);
        elf_header->ELF_PH_entry_size = revert_value<uint16_t> (make_into_complete_value<uint16_t> (2, read_in_data));
        check_data<uint16_t> (elf_header->ELF_PH_entry_size, ELF_PROGRAM_HEADER_SIZE);

        /* Get the amount of Program Header entries. */
        ELF_get_data(2, ELF_parts::ELF_Header, *read_in_data);
        elf_header->ELF_PH_entry_amnt = revert_value<uint16_t> (make_into_complete_value<uint16_t> (2, read_in_data));

        /* Get the Section Header size. */
        ELF_get_data(2, ELF_parts::ELF_Header, *read_in_data);
        elf_header->ELF_SH_size = revert_value<uint16_t> (make_into_complete_value<uint16_t> (2, read_in_data));
        check_data<uint16_t> (elf_header->ELF_SH_size, ELF_SECTION_HEADER_SIZE);

        /* Get the amount of Section Header entries. */
        ELF_get_data(2, ELF_parts::ELF_Header, *read_in_data);
        elf_header->ELF_SH_entry_amnt = revert_value<uint16_t> (make_into_complete_value<uint16_t> (2, read_in_data));

        /* Get the index of the entry associated with the section name string table. */
        ELF_get_data(2, ELF_parts::ELF_Header, *read_in_data);
        elf_header->ELF_SH_str_index = revert_value<uint16_t> (make_into_complete_value<uint16_t> (2, read_in_data));

        delete read_in_data;
    }

    printf("\nELF Signature: %X\nELF Bit Type: 0x%X (%s)\nELF Endianess: 0x%X (%s)\n",
        elf_header->ELF_magic,
        elf_header->ELF_type,
        get_ELF_type_name((ELF_types) elf_header->ELF_type),
        elf_header->ELF_endianess,
        get_ELF_endianess_name((ELF_endianess) elf_header->ELF_endianess));
    
    printf("ELF Version: 0x%X\nELF File Type: 0x%X (%s)\nELF Machine Type: 0x%X (%s)\n",
        elf_header->ELF_version,
        elf_header->ELF_file_type,
        get_ELF_file_type_name((ELF_file_types) elf_header->ELF_file_type),
        elf_header->ELF_machine_type,
        get_ELF_machine_type_name((ELF_machine_types) elf_header->ELF_machine_type));
    
    printf("ELF Entry: 0x%X\nELF Program Header Offset: 0x%X\n",
        elf_header->ELF_entry,
        elf_header->ELF_PH_offset);
    
    printf("ELF Section Header Offset: 0x%x\nELF Flags: 0x%X\nELF Header Size: 0x%X (%d bytes)\n",
        elf_header->ELF_SH_offset,
        elf_header->ELF_flags,
        elf_header->ELF_hsize,
        elf_header->ELF_hsize);
    
    printf("ELF Program Header Entry Size: 0x%X (%d bytes)\nELF Program Header Entry Amount: 0x%x (%d entries)\n",
        elf_header->ELF_PH_entry_size,
        elf_header->ELF_PH_entry_size,
        elf_header->ELF_PH_entry_amnt,
        elf_header->ELF_PH_entry_amnt);
    
    printf("ELF Section Header Size: 0x%X (%d bytes)\nELF Section Header Entry Amount: 0x%X (%d entries)\n",
        elf_header->ELF_SH_size,
        elf_header->ELF_SH_size,
        elf_header->ELF_SH_entry_amnt,
        elf_header->ELF_SH_entry_amnt);
    
    printf("ELF Section Header String Table Index: 0x%X\n\n",
        elf_header->ELF_SH_str_index);

    //ELF_get_data()

    return *elf_header;
}

/***********          END ELF HEADER          ***********/