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

#ifndef ELFRELOCATIONENTRY_H
#define ELFRELOCATIONENTRY_H

#include <QtGlobal>
#include <cstdint>

class ElfRelocationSection;
class ElfSymbolTableEntry;

class ElfRelocationEntry
{
public:
    ElfRelocationEntry();
    ElfRelocationEntry(const ElfRelocationEntry&);
    explicit ElfRelocationEntry(const ElfRelocationSection *section, uint64_t index, bool withAddend);
    ~ElfRelocationEntry();

    ElfRelocationEntry& operator=(const ElfRelocationEntry&);

    const ElfRelocationSection* relocationTable() const;

    uint64_t offset() const;
    uint32_t symbolIndex() const;
    uint32_t type() const;
    uint64_t addend() const;

    /** Symbol table entry referenced from this relocation, can be @c nullptr. */
    ElfSymbolTableEntry* symbol() const;

    /** Returns the address the relocation actually points too in the end.
     *  How this is computed depends on the type of relocation and the platform.
     */
    uint64_t relocationTarget() const;

private:
    template <typename T> const T* entry() const;
    bool is64() const;

    const ElfRelocationSection *m_section;
    uint64_t m_index:63;
    uint64_t m_withAddend:1;
};

Q_DECLARE_TYPEINFO(ElfRelocationEntry, Q_MOVABLE_TYPE);

#endif // ELFRELOCATIONENTRY_H
