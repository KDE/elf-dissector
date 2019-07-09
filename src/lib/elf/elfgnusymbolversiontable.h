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

#ifndef ELFGNUSYMBOLVERSIONTABLE_H
#define ELFGNUSYMBOLVERSIONTABLE_H

#include "elfsection.h"

/** GNU Symbol Version Table section. */
class ElfGNUSymbolVersionTable : public ElfSection
{
public:
    explicit ElfGNUSymbolVersionTable(ElfFile *file, ElfSectionHeader *shdr);
    ~ElfGNUSymbolVersionTable();

    /** Index into the version definition table. */
    uint16_t versionIndex(uint32_t index) const;

    /** Returns whether the hidden flag (bit 15) is set on this entry. */
    bool isHidden(uint32_t index) const;
};

#endif // ELFGNUSYMBOLVERSIONTABLE_H
