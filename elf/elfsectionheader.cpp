#include "elfsectionheader.h"
#include "elffile.h"
#include "elfheader.h"

#include <cassert>

ElfSectionHeader::ElfSectionHeader(ElfFile* file, uint16_t sectionIndex) :
    m_file(file), m_sectionIndex(sectionIndex)
{
    assert(file);
}

uint16_t ElfSectionHeader::sectionIndex() const
{
    return m_sectionIndex;
}

uint64_t ElfSectionHeader::headerOffset() const
{
    return m_file->header()->sectionHeaderTableOffset() + sectionIndex() * m_file->header()->sectionHeaderEntrySize();
}
