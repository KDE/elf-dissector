#include "elfsymboltablesection.h"
#include "elfsectionheader.h"

ElfSymbolTableSection::ElfSymbolTableSection(ElfFile* file, const ElfSectionHeader::Ptr& shdr): ElfArraySection(file, shdr)
{
}
