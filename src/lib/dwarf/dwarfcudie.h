/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
