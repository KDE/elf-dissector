#include "elfsymboltablesection.h"
#include "elfsectionheader.h"

ElfSymbolTableSection::ElfSymbolTableSection(ElfFile* file, const ElfSectionHeader::Ptr& shdr): ElfSection(file, shdr)
{
}

ElfSymbolTableSection::~ElfSymbolTableSection()
{
    m_entries.clear();
}

void ElfSymbolTableSection::parse()
{
    m_entries.reserve(header()->entryCount());
    for (uint32_t index = 0; index < header()->entryCount(); ++index)
        m_entries.push_back(createEntry(index));
}

ElfSymbolTableEntry::Ptr ElfSymbolTableSection::entry(uint32_t index) const
{
    return m_entries.at(index);
}
