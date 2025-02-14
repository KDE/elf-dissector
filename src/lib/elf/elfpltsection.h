/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFPLTSECTION_H
#define ELFPLTSECTION_H

#include "elfsection.h"
#include "elfpltentry.h"

#include <QList>

class ElfGotSection;

class ElfPltSection : public ElfSection
{
public:
    explicit ElfPltSection(ElfFile* file, ElfSectionHeader* shdr);
    ~ElfPltSection();

    ElfPltEntry* entry(uint64_t index) const;

    /** The GOT section used by this PLT section. */
    ElfGotSection* gotSection() const;

private:
    QList<ElfPltEntry> m_entries;
    mutable ElfGotSection *m_gotSection;
};

#endif // ELFPLTSECTION_H
