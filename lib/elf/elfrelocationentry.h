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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ELFRELOCATIONENTRY_H
#define ELFRELOCATIONENTRY_H

#include <QtGlobal>
#include <cstdint>

class ElfRelocationSection;

class ElfRelocationEntry
{
public:
    ElfRelocationEntry();
    explicit ElfRelocationEntry(const ElfRelocationSection *section, uint64_t index, bool withAddend);
    ~ElfRelocationEntry();

    const ElfRelocationSection* relocationTable() const;

    uint64_t offset() const;
    uint32_t symbol() const;
    uint32_t type() const;
    uint64_t addend() const;

private:
    template <typename T> const T* entry() const;
    bool is64() const;

    const ElfRelocationSection *m_section;
    uint64_t m_index:63;
    uint64_t m_withAddend:1;
};

Q_DECLARE_TYPEINFO(ElfRelocationEntry, Q_MOVABLE_TYPE);

#endif // ELFRELOCATIONENTRY_H
