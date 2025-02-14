/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
