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
