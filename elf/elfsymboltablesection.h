#ifndef ELFSYMBOLTABLESECTION_H
#define ELFSYMBOLTABLESECTION_H

#include "elfsection.h"

/** Size-independent adaptor for symbol table sections. */
class ElfSymbolTableSection : public ElfSection
{
public:
    explicit ElfSymbolTableSection(ElfFile *file, const ElfSectionHeader::Ptr &shdr);

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
    explicit inline ElfSymbolTableSectionImpl(ElfFile *file, const ElfSectionHeader::Ptr &shdr) : ElfSymbolTableSection(file, shdr) {}

    template <typename S>
    class ElfSymbolTableEntryImpl : public ElfSymbolTableEntry
    {
    public:
        explicit inline ElfSymbolTableEntryImpl(const unsigned char *data) : m_symbol(reinterpret_cast<const S*>(data)) {}

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
        const S* m_symbol = 0;
    };

    inline ElfSymbolTableSection::ElfSymbolTableEntry* entry(uint32_t index) const override
    {
        return new ElfSymbolTableEntryImpl<T>(rawData() + index * sizeof(T));
    }
};

#endif // ELFSYMBOLTABLESECTION_H
