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

#ifndef DWARFADDRESSRANGES_H
#define DWARFADDRESSRANGES_H

#include <libdwarf/libdwarf.h>

#include <cstdint>

class DwarfInfo;
class DwarfDie;

/** Representation of the .debug_aranges section. */
class DwarfAddressRanges
{
public:
    explicit DwarfAddressRanges(DwarfInfo *info);
    DwarfAddressRanges(const DwarfAddressRanges&) = delete;
    ~DwarfAddressRanges();

    DwarfAddressRanges& operator=(const DwarfAddressRanges&) = delete;

    /** Returns @c true if the .debug_aranges section is present. */
    bool isValid() const;

    /** Looks up the CU DIE for the given address. */
    DwarfDie* compilationUnitForAddress(uint64_t addr) const;

private:
    Dwarf_Arange *m_aranges;
    DwarfInfo *m_info;
    Dwarf_Signed m_arangesSize;
};

#endif // DWARFADDRESSRANGES_H
