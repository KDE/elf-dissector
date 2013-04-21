#ifndef ELFSYMBOLTABLESECTION_IMPL_H
#define ELFSYMBOLTABLESECTION_IMPL_H

#include "elfsymboltablesection.h"
#include "elfsymboltableentry_impl.h"

template <typename T>
class ElfSymbolTableSectionImpl : public ElfSymbolTableSection
{
public:
    explicit inline ElfSymbolTableSectionImpl(ElfFile *file, const ElfSectionHeader::Ptr &shdr) : ElfSymbolTableSection(file, shdr)
    {
        parse();
    }

    inline ElfSymbolTableEntry::Ptr createEntry(uint32_t index) const override
    {
        return ElfSymbolTableEntry::Ptr(new ElfSymbolTableEntryImpl<T>(this, index));
    }
};

#endif // ELFSYMBOLTABLESECTION_IMPL_H
