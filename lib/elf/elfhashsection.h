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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ELFHASHSECTION_H
#define ELFHASHSECTION_H

#include "elfsection.h"

#include <QVector>

class ElfSymbolTableEntry;

/** Represents the .hash section. */
class ElfHashSection : public ElfSection
{
public:
    ~ElfHashSection();
    ElfHashSection(ElfFile* file, ElfSectionHeader* shdr);

    uint32_t bucketCount() const;
    uint32_t bucket(uint32_t index) const;
    uint32_t chainCount() const;
    uint32_t chain(uint32_t index) const;

    static uint32_t elfHash(const char* name);
    ElfSymbolTableEntry *lookup(const char* name);

    QVector<uint32_t> histogram() const;
};

#endif // ELFHASHSECTION_H
