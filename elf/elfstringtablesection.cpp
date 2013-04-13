#include "elfstringtablesection.h"

#include <cassert>

ElfStringTableSection::ElfStringTableSection(unsigned char* data, uint64_t size): ElfSection(data, size)
{
}

const char* ElfStringTableSection::string(uint32_t index) const
{
    assert(index < m_size);
    return (const char*)(m_data + index);
}
