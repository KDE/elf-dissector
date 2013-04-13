#ifndef ELFSYMBOLTABLESECTION_H
#define ELFSYMBOLTABLESECTION_H

#include "elfsection.h"

/** Size-independent adaptor for symbol table sections. */
class ElfSymbolTableSection : public ElfSection
{
public:
    explicit ElfSymbolTableSection(unsigned char* data, uint64_t size);

    class ElfSymbolTableEntry
    {
    public:
        virtual ~ElfSymbolTableEntry();
        virtual uint32_t name() const = 0;
        virtual uint16_t sectionIndex() const = 0;
        virtual uint64_t size() const = 0;
    };

    virtual ElfSymbolTableEntry* entry(uint32_t index) const = 0;
};


template <typename T>
class ElfSymbolTableSectionImpl : public ElfSymbolTableSection
{
public:
    explicit inline ElfSymbolTableSectionImpl(unsigned char* data, uint64_t size) : ElfSymbolTableSection(data, size) {}

    template <typename S>
    class ElfSymbolTableEntryImpl : public ElfSymbolTableEntry
    {
    public:
        explicit inline ElfSymbolTableEntryImpl(unsigned char *data) : m_symbol(reinterpret_cast<S*>(data)) {}

        inline uint32_t name() const override
        {
            return m_symbol->st_name;
        }

        inline uint16_t sectionIndex() const override
        {
            return m_symbol->st_shndx;
        }

        inline uint64_t size() const override
        {
            return m_symbol->st_size;
        }

    private:
        S* m_symbol = 0;
    };

    inline ElfSymbolTableSection::ElfSymbolTableEntry* entry(uint32_t index) const override
    {
        return new ElfSymbolTableEntryImpl<T>(m_data + index * sizeof(T));
    }
};

#endif // ELFSYMBOLTABLESECTION_H
