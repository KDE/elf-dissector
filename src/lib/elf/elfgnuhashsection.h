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

#ifndef ELFGNUHASHSECTION_H
#define ELFGNUHASHSECTION_H

#include "elfhashsection.h"

#include <QVector>

class ElfSymbolTableEntry;

/** Represents the .gnu.hash section. */
class ElfGnuHashSection : public ElfHashSection
{
public:
    explicit ElfGnuHashSection(ElfFile* file, ElfSectionHeader* shdr);
    ~ElfGnuHashSection();

    uint32_t bucketCount() const final override;
    uint32_t chainCount() const final override;

    uint32_t symbolIndex() const;
    uint32_t maskWordsCount() const;
    uint32_t shift2() const;

    static uint32_t hash(const char* name);
    ElfSymbolTableEntry *lookup(const char* name) const final override;

    QVector<uint32_t> histogram() const final override;
    double averagePrefixLength() const final override;

private:
    uint32_t bucket(uint32_t index) const;
    uint32_t* value(uint32_t index) const;
    uint64_t filterMask(uint32_t index) const;
};

#endif // ELFGNUHASHSECTION_H
