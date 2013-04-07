#ifndef ELFHEADER_H
#define ELFHEADER_H

#include <cstdint>

/** Size-independent adaptor to ElfXX_Ehdr. */
class ElfHeader
{
public:
//     virtual uint16_t type() const = 0;
//     virtual uint16_t machine() const = 0;
//     virtual uint32_t version() const = 0;
//     virtual uint64_t entryPoint() const = 0;
//     virtual uint64_t programHeaderTableOffset() const = 0;
    virtual uint64_t sectionHeaderTableOffset() const = 0;
//     virtual uint32_t flags() const = 0;
//     virtual uint16_t headerSize() const = 0;
//     virtual uint16_t programHeaderEntrySize() const = 0;
//     virtual uint16_t programHeaderCount() const = 0;
    virtual uint16_t sectionHeaderEntrySize() const = 0;
    virtual uint16_t sectionHeaderCount() const = 0;
    virtual uint16_t stringTableSectionHeader() const = 0;
};

template <typename T>
class ElfHeaderImpl : public ElfHeader
{
public:
    explicit inline ElfHeaderImpl(unsigned char *data) : m_hdr(reinterpret_cast<T*>(data)) {}

    inline uint64_t sectionHeaderTableOffset() const override { return m_hdr->e_shoff; }
    inline uint16_t sectionHeaderEntrySize() const override { return m_hdr->e_shentsize; }
    inline uint16_t  sectionHeaderCount() const override { return m_hdr->e_shnum; }
    inline uint16_t stringTableSectionHeader() const override { return m_hdr->e_shstrndx; }

private:
    T* m_hdr;
};

#endif // ELFHEADER_H
