/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
    const auto res = dwarf_get_ranges_a(die->dwarfInfo()->dwarfHandle(), offset,
                                        die->dieHandle(), &ranges, &m_rangeSize,
                                        nullptr, nullptr);
    if (res != DW_DLV_OK)
        return;

    m_ranges.reset(ranges, [die, this](Dwarf_Ranges* ranges) {
        dwarf_ranges_dealloc(die->dwarfInfo()->dwarfHandle(), ranges, m_rangeSize);
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
