#include "elfsection.h"

ElfSection::ElfSection(unsigned char* data, uint64_t size) :
    m_data(data),
    m_size(size)
{
}

ElfSection::~ElfSection()
{
}

void ElfSection::setLinkedSection(ElfSection* linkedSection)
{
    m_linkedSection = linkedSection;
}
