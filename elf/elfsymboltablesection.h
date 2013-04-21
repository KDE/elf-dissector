#ifndef ELFSYMBOLTABLESECTION_H
#define ELFSYMBOLTABLESECTION_H

#include "elfarraysection.h"
#include "elfsymboltableentry.h"

/** Size-independent adaptor for symbol table sections. */
class ElfSymbolTableSection : public ElfArraySection<ElfSymbolTableEntry>
{
public:
    explicit ElfSymbolTableSection(ElfFile* file, const ElfSectionHeader::Ptr& shdr);
};

#endif // ELFSYMBOLTABLESECTION_H
