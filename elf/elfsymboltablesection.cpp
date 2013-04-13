#include "elfsymboltablesection.h"

ElfSymbolTableSection::ElfSymbolTableSection(unsigned char* data, uint64_t size): ElfSection(data, size)
{
}

ElfSymbolTableSection::ElfSymbolTableEntry::~ElfSymbolTableEntry()
{
}

