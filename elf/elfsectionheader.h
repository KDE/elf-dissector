#ifndef ELFSECTIONHEADER_H
#define ELFSECTIONHEADER_H

#include <cstdint>
#include <memory>

class ElfFile;

/** Size-independent adapter to ElfXX_Shdr. */
class ElfSectionHeader
{
public:
    typedef std::shared_ptr<ElfSectionHeader> Ptr;

    ElfSectionHeader(const ElfSectionHeader &other) = delete;
    ElfSectionHeader& operator=(const ElfSectionHeader &other) = delete;

    uint16_t sectionIndex() const;
    /** The location of the header (not the section it describes) in the ELF file. */
    uint64_t headerOffset() const;

    /** String index of the section name. */
    virtual uint32_t nameIndex() const = 0;
    virtual uint32_t type() const = 0;
    virtual uint64_t flags() const = 0;
//    virtual uint64_t address() const = 0;
    /** The location of the section (not this header) in the ELF file. */
    virtual uint64_t sectionOffset() const = 0;
    virtual uint64_t size() const = 0;
    virtual uint32_t link() const = 0;
//    virtual uint32_t info() const = 0;
//    virtual uint64_t alignment() const = 0;
    virtual uint64_t entrySize() const = 0;

    /** Section name. */
    const char* name() const;
    /** Returns @c true if this section is related to debug information in some way. */
    bool isDebugInformation() const;

    /** Returns the amount of entries in this section if section is an array. */
    uint64_t entryCount() const;

protected:
    explicit ElfSectionHeader(ElfFile *file, uint16_t sectionIndex);

private:
    ElfFile *m_file = 0;
    uint16_t m_sectionIndex = 0;
};

#endif // ELFSECTIONHEADER_H
