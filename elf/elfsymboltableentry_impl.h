#ifndef ELFSYMBOLTABLEENTRY_IMPL_H
#define ELFSYMBOLTABLEENTRY_IMPL_H

#include "elfsymboltableentry.h"
#include "elfsymboltablesection.h"

template <typename T>
class ElfSymbolTableEntryImpl : public ElfSymbolTableEntry
{
public:
    explicit inline ElfSymbolTableEntryImpl(const ElfSymbolTableSection* section, uint32_t index) :
    ElfSymbolTableEntry(section),
    m_symbol(reinterpret_cast<const T*>(section->rawData() + index * section->header()->entrySize()))
    {
    }

    inline uint32_t nameIndex() const override
    {
        return m_symbol->st_name;
    }

    inline uint8_t info() const override
    {
        return m_symbol->st_info;
    }

    inline uint8_t other() const override
    {
        return m_symbol->st_other;
    }

    inline uint16_t sectionIndex() const override
    {
        return m_symbol->st_shndx;
    }

    inline uint64_t value() const override
    {
        return m_symbol->st_value;
    }

    inline uint64_t size() const override
    {
        return m_symbol->st_size;
    }

private:
    const T* m_symbol = 0;
};

#endif // ELFSYMBOLTABLEENTRY_IMPL_H
