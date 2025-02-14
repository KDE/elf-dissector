/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dwarfline.h"

#include <cassert>

DwarfLine::DwarfLine(Dwarf_Line line) :
    m_line(line)
{
    assert(line);
}

bool DwarfLine::isNull() const
{
    return m_line == nullptr;
}

Dwarf_Unsigned DwarfLine::line() const
{
    Dwarf_Unsigned n = 0;
    if (dwarf_lineno(m_line, &n, nullptr) == DW_DLV_OK)
        return n;
    return 0;
}

Dwarf_Signed DwarfLine::column() const
{
    Dwarf_Signed n = 0;
    if (dwarf_lineoff(m_line, &n, nullptr) == DW_DLV_OK)
        return n;
    return 0;
}

Dwarf_Addr DwarfLine::address() const
{
    Dwarf_Addr addr = 0;
    if (dwarf_lineaddr(m_line, &addr, nullptr) == DW_DLV_OK)
        return addr;
    return 0;
}

Dwarf_Line DwarfLine::handle() const
{
    return m_line;
}
