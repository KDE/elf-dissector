/*
    Copyright (C) 2015 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef ELFREVERSERELOCATOR_H
#define ELFREVERSERELOCATOR_H

#include <QVector>

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

    QVector<ElfRelocationSection*> m_relocSections;
    mutable QVector<ElfRelocationEntry*> m_relocations;
};

#endif // ELFREVERSERELOCATOR_H
