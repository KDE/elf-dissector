#include "elfdynamicentry.h"

ElfDynamicEntry::ElfDynamicEntry(const ElfDynamicSection* section) : m_section(section)
{
}

ElfDynamicEntry::~ElfDynamicEntry()
{
}

const ElfDynamicSection* ElfDynamicEntry::dynamicSection() const
{
    return m_section;
}
