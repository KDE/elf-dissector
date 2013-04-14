#include "elfstringtablesection.h"

#include <cassert>

ElfStringTableSection::ElfStringTableSection(ElfFile* file, const ElfSectionHeader::Ptr& shdr): ElfSection(file, shdr)
{
}

const char* ElfStringTableSection::string(uint32_t index) const
{
    assert(index < size());
    return (const char*)(rawData() + index);
}
