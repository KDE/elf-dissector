#ifndef ELFDYNAMICENTRY_IMPL_H
#define ELFDYNAMICENTRY_IMPL_H

#include "elfdynamicentry.h"

template <typename T>
class ElfDynamicEntryImpl : public ElfDynamicEntry
{
public:
    explicit inline ElfDynamicEntryImpl(const ElfDynamicSection *section, uint32_t index) :
        ElfDynamicEntry(section),
        m_entry(reinterpret_cast<const T*>(section->rawData() + index * section->header()->entrySize()))
    {
    }

    int64_t tag() const override
    {
        return m_entry->d_tag;
    }

    uint64_t value() const override
    {
        return m_entry->d_un.d_val;
    }

    uint64_t pointer() const override
    {
        return m_entry->d_un.d_ptr;
    }

private:
    const T* m_entry = 0;

};

#endif