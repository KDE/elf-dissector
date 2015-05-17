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

ElfSymbolTableSection::ElfSymbolTableSection(ElfFile* file, ElfSectionHeader *shdr): ElfSection(file, shdr)
{
    m_entries.reserve(header()->entryCount());
    const auto entryCount = header()->entryCount();
    for (uint i = 0; i < entryCount; ++i)
        m_entries.push_back(ElfSymbolTableEntry(this, i));
}

ElfSymbolTableSection::~ElfSymbolTableSection() = default;

ElfSymbolTableEntry* ElfSymbolTableSection::entry(uint32_t index) const
{
    return const_cast<ElfSymbolTableEntry*>(m_entries.data() + index);
}

int ElfSymbolTableSection::exportCount() const
{
    int count = 0;
    for (auto it = m_entries.constBegin(); it != m_entries.constEnd(); ++it) {
        if (it->bindType() == STB_GLOBAL && it->size() > 0)
            ++count;
    }
    return count;
}

int ElfSymbolTableSection::importCount() const
{
    int count = 0;
    for (auto it = m_entries.constBegin(); it != m_entries.constEnd(); ++it) {
        if (it->bindType() == STB_GLOBAL && it->size() == 0)
            ++count;
    }
    return count;
}

ElfSymbolTableEntry* ElfSymbolTableSection::entryWithValue(uint64_t value) const
{
    if (value == 0)
        return nullptr;

    for (auto it = m_entries.constBegin(); it != m_entries.constEnd(); ++it) {
        if (it->value() == value)
            return const_cast<ElfSymbolTableEntry*>(it);
    }
    return nullptr;
}

ElfSymbolTableEntry* ElfSymbolTableSection::entryContainingValue(uint64_t value) const
{
    if (value == 0)
        return nullptr;

    for (auto it = m_entries.constBegin(); it != m_entries.constEnd(); ++it) {
        if (it->value() == 0 || it->size() == 0)
            continue;
        if (it->value() <= value && value < it->value() + it->size())
            return const_cast<ElfSymbolTableEntry*>(it);
    }
    return nullptr;
}
