#ifndef ELFSTRINGTABLESECTION_H
#define ELFSTRINGTABLESECTION_H

#include "elfsection.h"

/** String table section. */
class ElfStringTableSection : public ElfSection
{
public:
    explicit ElfStringTableSection(ElfFile *file, const ElfSectionHeader::Ptr &shdr);

    /** Returns the string at @p index. */
    const char* string(uint32_t index) const;
};

#endif // ELFSTRINGTABLESECTION_H
