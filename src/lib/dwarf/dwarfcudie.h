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

#ifndef DWARFCUDIE_H
#define DWARFCUDIE_H

#include "dwarfdie.h"

class DwarfInfo;
class DwarfLine;

class DwarfCuDie : public DwarfDie
{
public:
    ~DwarfCuDie();

    DwarfLine lineForAddress(Dwarf_Addr addr) const;
    QString sourceFileForLine(DwarfLine line) const;

protected:
    friend class DwarfDie;
    friend class DwarfInfoPrivate;
    explicit DwarfCuDie(Dwarf_Die die, DwarfInfo* info);

    const char* sourceFileForIndex(int i) const;

private:
    void loadLines() const;

private:
    mutable char** m_srcFiles = nullptr;
    mutable Dwarf_Signed m_srcFileCount = 0;

    mutable Dwarf_Line* m_lines = nullptr;
    mutable Dwarf_Signed m_lineCount = 0;
};

#endif // DWARFCUDIE_H
