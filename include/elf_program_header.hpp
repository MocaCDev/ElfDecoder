#ifndef ELF_PROGRAM_HEADER_H
#define ELF_PROGRAM_HEADER_H
#include "elf_header.hpp"
using namespace elf_header;

namespace elf_program_header
{
    enum class SegmentTypes: uint32_t
    {
        ST_NULL,    /* unused */
        ST_LOAD,    /* array element specifies loadable segment */
        ST_DYNAMIC, /* array element specifies dynamic linking information */
        ST_INTERP,  /* array element specifies location and size of null-terminated path. meaningful only for executable files */
        ST_NOTE,    /* array element specifies the location and size of auxiliar information */
        ST_SHLIB,   /* ignore */
        ST_PHDR,    /* array element, if present, specifies the location and size of the program header table itself */
        ST_LOPROC = 0x70000000,  /* processor-specific semantics */
        ST_HIPROC = 0x7FFFFFFF,  /* processor-specific semantics */
    };

    static uint8_t *get_entry_type_name(SegmentTypes stype)
    {
        switch(stype)
        {
            case SegmentTypes::ST_NULL: return (uint8_t *) "Unused Entry";break;
            case SegmentTypes::ST_LOAD: return (uint8_t *) "Entry Describing A Loadable Segment";break;
            case SegmentTypes::ST_DYNAMIC: return (uint8_t *) "Entry Describing Dynamic Linking Information";break;
            case SegmentTypes::ST_INTERP: return (uint8_t *) "Entry Describing Location & Size Of Null-Terminated Path Name To Invoke As Interpreter";break;
            case SegmentTypes::ST_NOTE: return (uint8_t *) "Entry Describing Location And Size Of Auxiliar Information";break;
            case SegmentTypes::ST_SHLIB: return (uint8_t *) "Reserved Entry Type";break;
            case SegmentTypes::ST_PHDR: return (uint8_t *) "Entry Describing Location And Size Of Program Header Table";break;
            case SegmentTypes::ST_LOPROC: return (uint8_t *) "Processor-specific";break;
            case SegmentTypes::ST_HIPROC: return (uint8_t *) "Processor-specific";break;
            default: break;
        }

        return (uint8_t *) "Unknown Segment Type";
    }

    enum class AlignmentTypes: uint32_t
    {
        NoAlignment1 = 0x00000000,
        NoAlignment2 = 0x00000001
    };

    class ElfProgramHeader : public ElfHeader
    {
    private:
        struct ProgramHeader
        {
            uint32_t        p_type;
            uint32_t        p_offset;
            uint32_t        p_virtual_address;
            uint32_t        p_physical_address;
            uint32_t        p_size;                 /* number of bytes in the file image of the segment */
            uint32_t        p_memory_size;          /* number of bytes in the memory image of the segment */
            uint32_t        p_flags;                /* flags relevant to the segment */
            uint32_t        p_align;

            ProgramHeader() = default;
            ~ProgramHeader() = default;
        };
    
    protected:
        struct ProgramHeader **pheader;
        uint16_t index;

    public:
        ElfProgramHeader() = default;
        ElfProgramHeader(FILE *f, int8_t &filename)
            : pheader(nullptr), index(0), ElfHeader(f, filename)
        {
            pheader = new struct ProgramHeader *;
            pheader[index] = new struct ProgramHeader;

            get_elf_header();
            print_elf_header();
        }

        void ELF_get_data(uint8_t length, ELF_parts part, uint8_t &dest);
        void get_program_header_table();
        void print_elf_program_header_table();

        ~ElfProgramHeader()
        {
            /* If a Program Header doesn't exist in a ELF binary file, `pheader` will be deleted in `get_program_header_entries`. */
            if(pheader)
            {
                for(uint16_t i = 0; i <= index; i++)
                {
                    delete pheader[index];
                    pheader[index] = nullptr;
                }
            
                delete pheader;
                pheader = nullptr;
            }
        }
    };
}

#endif