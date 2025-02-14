/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFSYMBOLTABLESECTION_H
#define ELFSYMBOLTABLESECTION_H

#include "elfarraysection.h"
#include "elfsymboltableentry.h"

#include <vector>

/** Represents a symbol table sections (.symtab or .dynsym). */
class ElfSymbolTableSection : public ElfSection
{
public:
    explicit ElfSymbolTableSection(ElfFile* file, ElfSectionHeader *shdr);
    ~ElfSymbolTableSection();

    /** Number of exported entries. */
    int exportCount() const;

    /** Number of undefined symbols, i.e. symbols needed to be provided from other libraries. */
    int importCount() const;

    /** Returns the symbol table at @p index. */
    ElfSymbolTableEntry* entry(uint32_t index) const;

    /** Finds the first symbol table entry with the given value.
     *  @return @c 0 if there is no matching entry.
     */
    ElfSymbolTableEntry* entryWithValue(uint64_t value) const;

    /** Similar as the above, but looks for entries containing @p value rather than matching it exactly .*/
    ElfSymbolTableEntry* entryContainingValue(uint64_t value) const;

private:
    // entries in order of occurrence
    std::vector<ElfSymbolTableEntry> m_entries;
    // entry pointers in order of their virtual address, for fast reverse lookup
    std::vector<ElfSymbolTableEntry*> m_entriesByValue;
};

#endif // ELFSYMBOLTABLESECTION_H
