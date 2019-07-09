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

#ifndef ELFNOTESECTION_H
#define ELFNOTESECTION_H

#include "elfsection.h"

#include <QVector>

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
    QVector<ElfNoteEntry*> m_entries;
};

#endif // ELFNOTESECTION_H
