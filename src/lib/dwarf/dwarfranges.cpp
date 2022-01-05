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

#include "dwarfranges.h"
#include "dwarfdie.h"
#include "dwarfinfo.h"

#include <cassert>

DwarfRanges::DwarfRanges() : m_rangeSize(0)
{
}

DwarfRanges::DwarfRanges(const DwarfDie* die, uint64_t offset)
{
    Dwarf_Ranges* ranges = nullptr;
    Dwarf_Off finaloffset = 0;
    const int res = dwarf_get_ranges_b(die->dwarfInfo()->dwarfHandle(), offset,
                                        die->dieHandle(), &finaloffset, &ranges, &m_rangeSize,
                                        nullptr, nullptr);

    if (res != DW_DLV_OK) {
        m_rangeSize = 0; // dwarf_get_ranges_b randomly sets this even when it fails
        return;
    }

    m_ranges.reset(ranges, [die, this](Dwarf_Ranges* ranges) {
        dwarf_dealloc_ranges(die->dwarfInfo()->dwarfHandle(), ranges, m_rangeSize);
    });
}

DwarfRanges::DwarfRanges(const DwarfRanges&) = default;
DwarfRanges::DwarfRanges(DwarfRanges&&) = default;
DwarfRanges::~DwarfRanges() = default;

DwarfRanges& DwarfRanges::operator=(const DwarfRanges&) = default;
DwarfRanges& DwarfRanges::operator=(DwarfRanges&&) = default;

bool DwarfRanges::isValid() const
{
    return m_ranges.get() != nullptr;
}

int DwarfRanges::size() const
{
    // ignore null terminator
    if (m_rangeSize > 0 && entry(m_rangeSize - 1)->dwr_type == DW_RANGES_END)
        return m_rangeSize - 1;
    return m_rangeSize;
}

Dwarf_Ranges* DwarfRanges::entry(int index) const
{
    return &m_ranges.get()[index];
}
