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

#ifndef ELFPLTENTRY_H
#define ELFPLTENTRY_H

#include <QtGlobal>

class ElfPltSection;
class ElfGotEntry;

class ElfPltEntry
{
public:
    ElfPltEntry() = default;
    ElfPltEntry(const ElfPltEntry&);
    explicit ElfPltEntry(ElfPltSection* section, uint64_t index);
    ~ElfPltEntry() = default;

    ElfPltEntry& operator=(const ElfPltEntry&);

    ElfPltSection* section() const;
    uint64_t index() const;
    const uchar* rawData() const;
    uint64_t size() const;

    /** The GOT entry used by this PLT entry. */
    ElfGotEntry* gotEntry() const;

private:
    ElfPltSection *m_section = nullptr;
    uint64_t m_index = 0;
};

Q_DECLARE_TYPEINFO(ElfPltEntry, Q_MOVABLE_TYPE);

#endif // ELFPLTENTRY_H
