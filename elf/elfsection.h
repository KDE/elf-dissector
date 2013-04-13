#ifndef ELFSECTION_H
#define ELFSECTION_H

#include <cstdint>

/** Base class for all ELF sections. */
class ElfSection
{
public:
    explicit ElfSection(unsigned char *data, uint64_t size);
    virtual ~ElfSection();

    template <typename T>
    inline T* linkedSection() { return static_cast<T*>(m_linkedSection); }
    void setLinkedSection(ElfSection* linkedSection);

protected:
    unsigned char *m_data = 0;
    uint64_t m_size = 0;
    ElfSection* m_linkedSection = 0;
};

#endif // ELFSECTION_H
