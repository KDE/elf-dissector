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

#include "elfrelocationsection.h"
#include "elfrelocationentry.h"
#include "elffile.h"

#include <elf.h>

ElfRelocationSection::ElfRelocationSection(ElfFile* file, ElfSectionHeader* shdr) :
    ElfSection(file, shdr)
{
    const auto size = shdr->entryCount();
    const auto withAddend = shdr->type() == SHT_RELA;
    m_entries.reserve(size);
    for (uint i = 0; i < size; ++i)
        m_entries.push_back(ElfRelocationEntry(this, i, withAddend));
}

ElfRelocationSection::~ElfRelocationSection() = default;

ElfRelocationEntry* ElfRelocationSection::entry(uint32_t index) const
{
    return const_cast<ElfRelocationEntry*>(m_entries.data() + index);
}
