#include <elf_program_header.hpp>
using namespace elf_program_header;

void ElfProgramHeader::ELF_get_data(uint8_t length, ELF_parts part, uint8_t &dest)
{
    if(!(part == ELF_parts::ELF_PHeader))
        return;
    
    if(length == 0)
        length = 1;
    
    length = edecoder->ELF_read_binary(length, &dest, *edecoder->pholder);
}

void ElfProgramHeader::get_program_header_table()
{
    
    uint8_t *read_in_data = nullptr;

    const auto get_four_bytes = [&read_in_data, this] ()
    {
        if(read_in_data == nullptr)
            read_in_data = new uint8_t[4];
        
        ELF_get_data(4, ELF_parts::ELF_PHeader, *read_in_data);
    };

    /*
     * TODO: remove the argument `uint32_t &dest` and make it to where `make_comp`
     *       returns the above code? That way, below, it looks like:
     *       `pheader[index]->variable = make_comp();`
     * 
     * */
    const auto make_comp = [&read_in_data, this] (uint32_t &dest)
    {
        dest = revert_value<uint32_t> (edecoder->make_into_complete_value<uint32_t> (4, read_in_data));
    };

    reloop:

    /* Segment Type. */
    get_four_bytes();
    make_comp(pheader[index]->p_type);

    /* Program Header Offset (should be 0x34). */
    get_four_bytes();
    make_comp(pheader[index]->p_offset);

    /* Virtual/Physical Address. */
    {
        get_four_bytes();
        make_comp(pheader[index]->p_virtual_address);

        get_four_bytes();
        make_comp(pheader[index]->p_physical_address);
    }

    /* Size (in bytes)/Memory size (in bytes) that the entries take up. */
    {
        get_four_bytes();
        make_comp(pheader[index]->p_size);

        get_four_bytes();
        make_comp(pheader[index]->p_memory_size);
    }

    /* Flags. */
    get_four_bytes();
    make_comp(pheader[index]->p_flags);

    /* Alignment. */
    get_four_bytes();
    make_comp(pheader[index]->p_align);

    if(pheader[0]->p_type == (uint32_t) SegmentTypes::ST_NULL)
    {
        delete pheader[0];
        delete pheader;
        pheader = nullptr;

        goto end;
    }

    if(pheader[index]->p_type != (uint32_t) SegmentTypes::ST_NULL) goto reloop;

    end:
    if(read_in_data) delete read_in_data;
    read_in_data = nullptr;
}

void ElfProgramHeader::print_elf_program_header_table()
{
    if(pheader)
    {
        for(uint16_t i = 0; i < index; i++)
        {
            printf("\tProgram Header Entry #%d:", i + 1);
            printf("\n\t\tEntry Type: %X (%s)",
                pheader[i]->p_type, get_entry_type_name((SegmentTypes) pheader[i]->p_type));
        }
    }
}