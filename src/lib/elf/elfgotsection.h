/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFGOTSECTION_H
#define ELFGOTSECTION_H

#include "elfsection.h"
#include "elfgotentry.h"

#include <QList>

/** Represents a .got.* section. */
class ElfGotSection : public ElfSection
{
public:
    explicit ElfGotSection(ElfFile* file, ElfSectionHeader* shdr);
    ~ElfGotSection();

    ElfGotEntry* entry(uint64_t index) const;
    uint64_t entryCount() const;

private:
    QList<ElfGotEntry> m_entries;
};

#endif // ELFGOTSECTION_H
