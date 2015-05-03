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

#include "dwarfaddressranges.h"
#include "dwarfinfo.h"

#include <QDebug>
#include <cassert>

DwarfAddressRanges::DwarfAddressRanges(DwarfInfo* info) :
    m_aranges(nullptr),
    m_info(info)
{
    assert(info);
    const auto res = dwarf_get_aranges(info->dwarfHandle(), &m_aranges, &m_arangesSize, nullptr);
    if (res != DW_DLV_OK)
        return;
}

DwarfAddressRanges::~DwarfAddressRanges()
{
    for (int i = 0; i < m_arangesSize; ++i)
        dwarf_dealloc(m_info->dwarfHandle(), m_aranges[i], DW_DLA_ARANGE);
    dwarf_dealloc(m_info->dwarfHandle(), m_aranges, DW_DLA_LIST);
}

bool DwarfAddressRanges::isValid() const
{
    return m_aranges;
}

DwarfDie* DwarfAddressRanges::compilationUnitForAddress(uint64_t addr) const
{
    if (!isValid())
        return nullptr;

    Dwarf_Arange arange;
    auto res = dwarf_get_arange(m_aranges, m_arangesSize, addr, &arange, nullptr);
    if (res != DW_DLV_OK)
        return nullptr;

    Dwarf_Off offset;
    res = dwarf_get_cu_die_offset(arange, &offset, nullptr);
    if (res != DW_DLV_OK)
        return nullptr;

    return m_info->dieAtOffset(offset);
}
