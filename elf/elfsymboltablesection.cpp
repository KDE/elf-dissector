#include "elfsymboltablesection.h"

ElfSymbolTableSection::ElfSymbolTableSection(ElfFile* file, const ElfSectionHeader::Ptr& shdr): ElfSection(file, shdr)
{
}

ElfSymbolTableSection::ElfSymbolTableEntry::~ElfSymbolTableEntry()
{
}

