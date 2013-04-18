#include "elfsection.h"
#include "elffile.h"

#include <cassert>

ElfSection::ElfSection(ElfFile* file, const ElfSectionHeader::Ptr& shdr) :
    m_file(file),
    m_sectionHeader(shdr)
{
}

ElfSection::~ElfSection()
{
}

void ElfSection::setLinkedSection(const ElfSection::Ptr &linkedSection)
{
    m_linkedSection = linkedSection;
}

uint64_t ElfSection::size() const
{
    return m_sectionHeader->size();
}

const unsigned char* ElfSection::rawData() const
{
    assert(m_file->size() > m_sectionHeader->size());
    return m_file->rawData() + m_sectionHeader->sectionOffset();
}

ElfFile* ElfSection::file() const
{
    return m_file;
}

ElfSectionHeader::Ptr ElfSection::header() const
{
    return m_sectionHeader;
}
