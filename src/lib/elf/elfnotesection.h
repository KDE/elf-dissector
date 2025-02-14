/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFNOTESECTION_H
#define ELFNOTESECTION_H

#include "elfsection.h"

#include <QList>

class ElfNoteEntry;

class ElfNoteSection : public ElfSection
{
public:
    explicit ElfNoteSection(ElfFile *file, ElfSectionHeader *shdr);
    ~ElfNoteSection();

    /** Number of note entries in this section. */
    int entryCount() const;

    ElfNoteEntry* entry(int index) const;

private:
    QList<ElfNoteEntry*> m_entries;
};

#endif // ELFNOTESECTION_H
