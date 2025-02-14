/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFREVERSERELOCATOR_H
#define ELFREVERSERELOCATOR_H

#include <QList>

class ElfRelocationEntry;
class ElfRelocationSection;

/** Look up if a given address is relocated. */
class ElfReverseRelocator
{
public:
    /** Total amount of relocations. */
    int size() const;

    /** Finds the relocation entry for the given virtual address.
     *  Returns @c nullptr if @p vaddr isn't relocated.
     */
    ElfRelocationEntry* find(uint64_t vaddr) const;

    /** Counts the amount of relocations within the given address range. */
    int relocationCount(uint64_t beginVAddr, uint64_t length) const;

    // internal for ElfFile
    void addRelocationSection(ElfRelocationSection* section);

private:
    void indexRelocations() const;

    QList<ElfRelocationSection*> m_relocSections;
    mutable QList<ElfRelocationEntry*> m_relocations;
};

#endif // ELFREVERSERELOCATOR_H
