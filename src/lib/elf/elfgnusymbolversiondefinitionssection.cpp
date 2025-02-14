/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "elfgnusymbolversiondefinitionssection.h"
#include "elfgnusymbolversiondefinition.h"
#include "elffile.h"

#include <elf.h>

ElfGNUSymbolVersionDefinitionsSection::ElfGNUSymbolVersionDefinitionsSection(ElfFile* file, ElfSectionHeader* shdr):
    ElfSection(file, shdr)
{
}

ElfGNUSymbolVersionDefinitionsSection::~ElfGNUSymbolVersionDefinitionsSection()
{
    qDeleteAll(m_versionDefinitions);
}

uint32_t ElfGNUSymbolVersionDefinitionsSection::entryCount() const
{
    return m_versionDefinitions.size();
}

ElfGNUSymbolVersionDefinition* ElfGNUSymbolVersionDefinitionsSection::definition(uint32_t index) const
{
    return m_versionDefinitions.at(index);
}

ElfGNUSymbolVersionDefinition* ElfGNUSymbolVersionDefinitionsSection::definitionForVersionIndex(uint16_t index) const
{
    for (auto def : m_versionDefinitions) {
        if ((def->versionIndex() & 0x7FFF) == (index & 0x7FFF))
            return def;
    }
    return nullptr;
}

void ElfGNUSymbolVersionDefinitionsSection::parse()
{
    // TODO parse until nextOffset() is 0 might be an alternative, removes dependency on dynamicSection() being available here
    const auto verDefNum = file()->dynamicSection()->entryWithTag(DT_VERDEFNUM);
    if (!verDefNum)
        return;

    uint32_t offset = 0;
    m_versionDefinitions.reserve(verDefNum->value());
    for (uint i = 0; i < verDefNum->value(); ++i) {
        const auto entry = new ElfGNUSymbolVersionDefinition(this, offset);
        m_versionDefinitions.push_back(entry);
        offset += entry->nextOffset();
    }
}
