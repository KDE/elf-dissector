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

#include "elfgotsection.h"
#include "elffile.h"

ElfGotSection::ElfGotSection(ElfFile* file, ElfSectionHeader* shdr) :
    ElfSection(file, shdr)
{
    const auto count = shdr->size() / file->addressSize();
    m_entries.reserve(count);
    for (uint64_t i = 0; i < count; ++i) {
        m_entries.push_back(ElfGotEntry(this, i));
    }
}

ElfGotSection::~ElfGotSection() = default;

ElfGotEntry* ElfGotSection::entry(uint64_t index) const
{
    return const_cast<ElfGotEntry*>(m_entries.data() + index);
}

uint64_t ElfGotSection::entryCount() const
{
    return header()->size() / file()->addressSize();
}
