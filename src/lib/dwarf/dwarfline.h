/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
