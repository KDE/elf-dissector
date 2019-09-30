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

#ifndef DWARFLINE_H
#define DWARFLINE_H

#include <libdwarf.h>


/** Represents one line information entry in the .debug_lines section. */
class DwarfLine
{
public:
    DwarfLine() = default;
    DwarfLine(const DwarfLine &other) = default;
    ~DwarfLine() = default;

    DwarfLine& operator=(const DwarfLine &other) = default;

    bool isNull() const;

    Dwarf_Unsigned line() const;
    Dwarf_Signed column() const;
    Dwarf_Addr address() const;

protected:
    friend class DwarfCuDie;
    DwarfLine(Dwarf_Line line);
    Dwarf_Line handle() const;

private:
    Dwarf_Line m_line = nullptr;
};

#endif // DWARFLINE_H
