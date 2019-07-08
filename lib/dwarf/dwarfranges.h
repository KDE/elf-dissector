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

#ifndef DWARFRANGES_H
#define DWARFRANGES_H

#include <QMetaType>

#include <libdwarf.h>

#include <memory>

class DwarfDie;

/** Represents the value of an DW_AT_ranges attribute. */
class DwarfRanges
{
public:
    DwarfRanges();
    explicit DwarfRanges(const DwarfDie* die, uint64_t offset);
    DwarfRanges(const DwarfRanges &other);
    DwarfRanges(DwarfRanges &&other);
    ~DwarfRanges();

    DwarfRanges& operator=(const DwarfRanges &other);
    DwarfRanges& operator=(DwarfRanges &&other);

    bool isValid() const;
    int size() const;
    Dwarf_Ranges* entry(int index) const;

private:
    std::shared_ptr<Dwarf_Ranges> m_ranges;
    Dwarf_Signed m_rangeSize;
};

Q_DECLARE_METATYPE(DwarfRanges)

#endif // DWARFRANGES_H
