#ifndef ELFSECTION_H
#define ELFSECTION_H

#include "elfsectionheader.h"

#include <cstdint>
#include <memory>

class ElfFile;

/** Base class for all ELF sections. */
class ElfSection
{
public:
    typedef std::shared_ptr<ElfSection> Ptr;

    explicit ElfSection(ElfFile *file, const ElfSectionHeader::Ptr &shdr);
    ElfSection(const ElfSection &other) = delete;
    virtual ~ElfSection();
    ElfSection& operator=(const ElfSection &other) = delete;

    template <typename T>
    inline std::shared_ptr<T> linkedSection() const
    {
        return std::dynamic_pointer_cast<T>(m_linkedSection);
    }
    void setLinkedSection(const ElfSection::Ptr &linkedSection);

    /** Size of the section. */
    uint64_t size() const;
    /** Access to the raw data of the section. */
    const unsigned char* rawData() const;

    /** The file this section belongs to. */
    ElfFile* file() const;
    /** Returns the corresponding section header. */
    ElfSectionHeader::Ptr header() const;

protected:
    ElfFile *m_file;
    ElfSectionHeader::Ptr m_sectionHeader;
    ElfSection::Ptr m_linkedSection;
};

#endif // ELFSECTION_H
