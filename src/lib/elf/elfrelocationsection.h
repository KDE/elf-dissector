/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFRELOCATIONSECTION_H
#define ELFRELOCATIONSECTION_H

#include "elfsection.h"
#include "elfrelocationentry.h"

#include <QList>

/** Relocation table sections. */
class ElfRelocationSection : public ElfSection
{
public:
    explicit ElfRelocationSection(ElfFile* file, ElfSectionHeader *shdr);
    ~ElfRelocationSection();

    ElfRelocationEntry* entry(uint32_t index) const;

private:
    QList<ElfRelocationEntry> m_entries;
};

#endif // ELFRELOCATIONSECTION_H
