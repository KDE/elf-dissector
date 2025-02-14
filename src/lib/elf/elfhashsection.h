/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFHASHSECTION_H
#define ELFHASHSECTION_H

#include "elfsection.h"

#include <QList>

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
    virtual QList<uint32_t> histogram() const = 0;
    /** Average length of common prefixes in case of hash collisions. */
    virtual double averagePrefixLength() const = 0;

protected:
    static int commonPrefixLength(const char *s1, const char *s2);
};

#endif // ELFHASHSECTION_H
