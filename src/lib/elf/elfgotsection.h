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

#ifndef ELFGOTSECTION_H
#define ELFGOTSECTION_H

#include "elfsection.h"
#include "elfgotentry.h"

#include <QVector>

/** Represents a .got.* section. */
class ElfGotSection : public ElfSection
{
public:
    explicit ElfGotSection(ElfFile* file, ElfSectionHeader* shdr);
    ~ElfGotSection();

    ElfGotEntry* entry(uint64_t index) const;
    uint64_t entryCount() const;

private:
    QVector<ElfGotEntry> m_entries;
};

#endif // ELFGOTSECTION_H
