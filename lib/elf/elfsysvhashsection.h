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

#ifndef ELFSYSVHASHSECTION_H
#define ELFSYSVHASHSECTION_H

#include "elfhashsection.h"

/** Represents the .hash section. */
class ElfSysvHashSection : public ElfHashSection
{
public:
    explicit ElfSysvHashSection(ElfFile* file, ElfSectionHeader* shdr);
    ~ElfSysvHashSection();

    uint32_t bucketCount() const final;
    uint32_t chainCount() const final;

    static uint32_t hash(const char* name);
    ElfSymbolTableEntry *lookup(const char* name) const final;

    QVector<uint32_t> histogram() const final;

private:
    uint32_t bucket(uint32_t index) const;
    uint32_t chain(uint32_t index) const;
};

#endif // ELFSYSVHASHSECTION_H
