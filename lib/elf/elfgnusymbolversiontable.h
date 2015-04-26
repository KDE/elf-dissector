#ifndef ELFGNUSYMBOLVERSIONTABLE_H
#define ELFGNUSYMBOLVERSIONTABLE_H

#include "elfsection.h"

/** GNU Symbol Version Table section. */
class ElfGNUSymbolVersionTable : public ElfSection
{
public:
    explicit ElfGNUSymbolVersionTable(ElfFile *file, ElfSectionHeader *shdr);
    ~ElfGNUSymbolVersionTable();

    /** Index into the version definition table. */
    uint16_t versionIndex(uint32_t index) const;
};

#endif // ELFGNUSYMBOLVERSIONTABLE_H
