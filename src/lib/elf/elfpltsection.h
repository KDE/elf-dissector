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

#ifndef ELFPLTSECTION_H
#define ELFPLTSECTION_H

#include "elfsection.h"
#include "elfpltentry.h"

#include <QVector>

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
    QVector<ElfPltEntry> m_entries;
    mutable ElfGotSection *m_gotSection;
};

#endif // ELFPLTSECTION_H
