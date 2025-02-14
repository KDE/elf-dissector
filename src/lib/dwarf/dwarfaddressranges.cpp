/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dwarfaddressranges.h"
#include "dwarfinfo.h"
#include "dwarfcudie.h"

#include <dwarf.h>

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

DwarfCuDie* DwarfAddressRanges::compilationUnitForAddress(uint64_t addr) const
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

    auto die = m_info->dieAtOffset(offset);
    if (!die)
        return nullptr;

    assert(die->isCompilationUnit());
    return static_cast<DwarfCuDie*>(die);
}

static DwarfDie* findByLowPCRecursive(DwarfDie *die, uint64_t addr)
{
    const auto lowPC = die->attribute(DW_AT_low_pc).toULongLong();
    if (lowPC == addr)
        return die;

    for (const auto child : die->children()) {
        auto res = findByLowPCRecursive(child, addr);
        if (res)
            return res;
    }

    return nullptr;
}

DwarfDie* DwarfAddressRanges::dieForAddress(uint64_t addr) const
{
    const auto cu = compilationUnitForAddress(addr);
    if (!cu)
        return nullptr;

    for (const auto die : cu->children()) {
        auto res = findByLowPCRecursive(die, addr);
        if (res)
            return res;
    }

    return nullptr;
}
