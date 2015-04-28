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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "elfgnusymbolversiondefinitionssection.h"
#include "elfgnusymbolversiondefinition.h"
#include "elffile.h"

#include <elf.h>

ElfGNUSymbolVersionDefinitionsSection::ElfGNUSymbolVersionDefinitionsSection(ElfFile* file, ElfSectionHeader* shdr):
    ElfSection(file, shdr)
{
}

ElfGNUSymbolVersionDefinitionsSection::~ElfGNUSymbolVersionDefinitionsSection() = default;

uint32_t ElfGNUSymbolVersionDefinitionsSection::entryCount() const
{
    return m_versionDefinitions.size();
}

ElfGNUSymbolVersionDefinition* ElfGNUSymbolVersionDefinitionsSection::definition(uint32_t index) const
{
    return m_versionDefinitions.at(index);
}

void ElfGNUSymbolVersionDefinitionsSection::parse()
{
    // TODO parse until nextOffset() is 0 might be an alternative, removes dependency on dynamicSection() being avaiable here
    const auto verDefNum = file()->dynamicSection()->entryWithTag(DT_VERDEFNUM);
    if (!verDefNum)
        return;

    uint32_t offset = 0;
    for (uint i = 0; i < verDefNum->value(); ++i) {
        const auto entry = new ElfGNUSymbolVersionDefinition(this, offset);
        m_versionDefinitions.push_back(entry);
        offset += entry->nextOffset();
    }
}