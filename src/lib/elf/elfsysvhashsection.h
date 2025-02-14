/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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

    uint32_t bucketCount() const final override;
    uint32_t chainCount() const final override;

    static uint32_t hash(const char* name);
    ElfSymbolTableEntry *lookup(const char* name) const final override;

    QList<uint32_t> histogram() const final override;
    double averagePrefixLength() const final override;

private:
    uint32_t bucket(uint32_t index) const;
    uint32_t chain(uint32_t index) const;
};

#endif // ELFSYSVHASHSECTION_H
