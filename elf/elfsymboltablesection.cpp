/*
    Copyright (C) 2013-2014 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "elfsymboltablesection.h"
#include "elfsectionheader.h"

#include <elf.h>

ElfSymbolTableSection::ElfSymbolTableSection(ElfFile* file, const ElfSectionHeader::Ptr& shdr): ElfArraySection(file, shdr)
{
}

int ElfSymbolTableSection::exportCount() const
{
    int count = 0;
    for (const auto entry : m_entries) {
        if (entry->bindType() == STB_GLOBAL && entry->size() > 0)
            ++count;
    }
    return count;
}

int ElfSymbolTableSection::importCount() const
{
    int count = 0;
    for (const auto entry : m_entries) {
        if (entry->bindType() == STB_GLOBAL && entry->size() == 0)
            ++count;
    }
    return count;
}
