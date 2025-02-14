/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFGNUHASHSECTION_H
#define ELFGNUHASHSECTION_H

#include "elfhashsection.h"

#include <QList>

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

    QList<uint32_t> histogram() const final override;
    double averagePrefixLength() const final override;

private:
    uint32_t bucket(uint32_t index) const;
    uint32_t* value(uint32_t index) const;
    uint64_t filterMask(uint32_t index) const;
};

#endif // ELFGNUHASHSECTION_H
