#ifndef ELFSECTIONHEADER_H
#define ELFSECTIONHEADER_H

#include <cstdint>

/** Size-independent adapter to ElfXX_Shdr. */
class ElfSectionHeader
{
public:
   virtual uint32_t name() const = 0;
   virtual uint32_t type() const = 0;
   virtual uint64_t flags() const = 0;
//    virtual uint64_t address() const = 0;
   virtual uint64_t offset() const = 0;
   virtual uint64_t size() const = 0;
//    virtual uint32_t link() const = 0;
//    virtual uint32_t info() const = 0;
//    virtual uint64_t alignment() const = 0;
//    virtual uint64_t entrySize() const = 0;
};

template <typename T>
class ElfSectionHeaderImpl : public ElfSectionHeader
{
public:
    inline ElfSectionHeaderImpl(unsigned char *data) : m_hdr(reinterpret_cast<T*>(data)) {}

    inline uint32_t name() const override { return m_hdr->sh_name; }
    inline uint32_t type() const override { return m_hdr->sh_type; }
    inline uint64_t flags() const override { return m_hdr->sh_flags; }

    inline uint64_t offset() const override { return m_hdr->sh_offset; }
    inline uint64_t size() const override { return m_hdr->sh_size; }

private:
    T* m_hdr;
};

#endif // ELFSECTIONHEADER_H
