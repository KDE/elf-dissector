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

#include "dwarfcudie.h"
#include "dwarfline.h"

#include <libdwarf.h>

#include <QFileInfo>

DwarfCuDie::DwarfCuDie(Dwarf_Die die, DwarfInfo* info) : DwarfDie(die, info)
{

}

DwarfCuDie::~DwarfCuDie()
{
    for (int i = 0; i < m_srcFileCount; ++i) {
        dwarf_dealloc(dwarfHandle(), m_srcFiles[i], DW_DLA_STRING);
    }
    dwarf_dealloc(dwarfHandle(), m_srcFiles, DW_DLA_LIST);

    for (int i = 0; i < m_lineCount; ++i) {
        dwarf_dealloc(dwarfHandle(), m_lines[i], DW_DLA_LINE);
    }
    dwarf_dealloc(dwarfHandle(), m_lines, DW_DLA_LIST);

    dwarf_dealloc(dwarfHandle(), m_die, DW_DLA_DIE);
}

const char* DwarfCuDie::sourceFileForIndex(int sourceIndex) const
{
    if (!m_srcFiles) {
        auto res = dwarf_srcfiles(m_die, &m_srcFiles, &m_srcFileCount, nullptr);
        if (res != DW_DLV_OK)
            return nullptr;
    }

    Q_ASSERT(sourceIndex >= 0);
    Q_ASSERT(sourceIndex < m_srcFileCount);
    return m_srcFiles[sourceIndex];
}

void DwarfCuDie::loadLines() const
{
    if (m_lines)
        return;

    dwarf_srclines(m_die, &m_lines, &m_lineCount, nullptr);
}

DwarfLine DwarfCuDie::lineForAddress(Dwarf_Addr addr) const
{
    loadLines();
    // ### is this sorted in any way?
    for (int i = 0; i < m_lineCount; ++i) {
        DwarfLine line(m_lines[i]);
        if (line.address() == addr)
            return line;
    }

    return {};
}

QString DwarfCuDie::sourceFileForLine(DwarfLine line) const
{
    char* srcFile = nullptr;
    auto res = dwarf_linesrc(line.handle(), &srcFile, nullptr);
    if (res != DW_DLV_OK)
        return {};
    auto fileName = QString::fromUtf8(srcFile);
    dwarf_dealloc(dwarfHandle(), srcFile, DW_DLA_STRING);

    QFileInfo fi(fileName);
    if (fi.exists())
        return fi.canonicalFilePath();
    return fileName;
}
