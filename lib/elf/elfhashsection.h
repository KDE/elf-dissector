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

#ifndef ELFHASHSECTION_H
#define ELFHASHSECTION_H

#include "elfsection.h"

#include <QVector>

class ElfSymbolTableEntry;

/** Interface for hash table sections for symbol lookup. */
class ElfHashSection : public ElfSection
{
public:
    ElfHashSection(ElfFile* file, ElfSectionHeader* shdr);
    ~ElfHashSection();

    virtual uint32_t bucketCount() const = 0;
    virtual uint32_t chainCount() const = 0;

    virtual ElfSymbolTableEntry *lookup(const char* name) const = 0;

    /** Histogram of the hash chain lengths. */
    virtual QVector<uint32_t> histogram() const = 0;
    /** Average length of common prefixes in case of hash collisions. */
    virtual double averagePrefixLength() const = 0;

protected:
    static int commonPrefixLength(const char *s1, const char *s2);
};

#endif // ELFHASHSECTION_H
