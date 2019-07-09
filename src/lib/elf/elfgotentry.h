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

#ifndef ELFGOTENTRY_H
#define ELFGOTENTRY_H

#include <QtGlobal>

class ElfGotSection;
class ElfRelocationEntry;

/** Entry in the global offset table. */
class ElfGotEntry
{
public:
    ElfGotEntry();
    explicit ElfGotEntry(ElfGotSection *section, uint64_t index);

    ElfGotSection *section() const;
    uint64_t index() const;
    uint64_t address() const;

    /** Relocation information for this GOT entry, can be @c nullptr. */
    ElfRelocationEntry* relocation() const;

private:
    ElfGotSection *m_section;
    uint64_t m_index;
};

Q_DECLARE_TYPEINFO(ElfGotEntry, Q_MOVABLE_TYPE);

#endif // ELFGOTENTRY_H
