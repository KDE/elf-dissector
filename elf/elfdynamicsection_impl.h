#ifndef ELFDYNAMICSECTION_IMPL_H
#define ELFDYNAMICSECTION_IMPL_H

#include "elfdynamicsection.h"
#include "elfdynamicentry_impl.h"

template <typename T>
class ElfDynamicSectionImpl : public ElfDynamicSection
{
public:
    explicit inline ElfDynamicSectionImpl(ElfFile *file, const ElfSectionHeader::Ptr &shdr) : ElfDynamicSection(file, shdr)
    {
        parse();
    }

    inline ElfDynamicEntry::Ptr createEntry(uint32_t index) const override
    {
        return ElfDynamicEntry::Ptr(new ElfDynamicEntryImpl<T>(this, index));
    }
};

#endif // ELFDYNAMICSECTION_IMPL_H
