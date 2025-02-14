/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "elfgnusymbolversiondefinition.h"
#include "elfgnusymbolversiondefinitionauxiliaryentry.h"

#include <elf.h>

#include <cassert>

ElfGNUSymbolVersionDefinition::ElfGNUSymbolVersionDefinition(ElfGNUSymbolVersionDefinitionsSection* section, uint32_t offset) :
    m_section(section),
    m_verDef(reinterpret_cast<const Elf64_Verdef*>(section->rawData() + offset))
{
    // 32bit and 64bit have exactly the same memory layout
    static_assert(sizeof(Elf32_Verdef) == sizeof(Elf64_Verdef), "SHT_GNU_verdef memory layout changed");

    assert(m_verDef->vd_version == 1);
    static_assert(VER_DEF_CURRENT == 1, "SHT_GNU_verdef format changed!");

    uint32_t auxOffset = 0;
    m_auxEntries.reserve(auxiliarySize());
    for (int i = 0; i < auxiliarySize(); ++i) {
        const auto auxEntry = new ElfGNUSymbolVersionDefinitionAuxiliaryEntry(this, auxOffset);
        m_auxEntries.push_back(auxEntry);
        auxOffset += auxEntry->nextAuxiliaryEntryOffset();
    }

    assert(auxiliarySize() == m_auxEntries.size());
}

ElfGNUSymbolVersionDefinition::~ElfGNUSymbolVersionDefinition()
{
    qDeleteAll(m_auxEntries);
}

ElfGNUSymbolVersionDefinitionsSection* ElfGNUSymbolVersionDefinition::section() const
{
    return m_section;
}

uint16_t ElfGNUSymbolVersionDefinition::flags() const
{
    return m_verDef->vd_flags;
}

uint16_t ElfGNUSymbolVersionDefinition::versionIndex() const
{
    return m_verDef->vd_ndx;
}

uint16_t ElfGNUSymbolVersionDefinition::auxiliarySize() const
{
    return m_verDef->vd_cnt;
}

uint32_t ElfGNUSymbolVersionDefinition::hash() const
{
    return m_verDef->vd_hash;
}

uint32_t ElfGNUSymbolVersionDefinition::auxiliaryOffset() const
{
    return m_verDef->vd_aux;
}

uint32_t ElfGNUSymbolVersionDefinition::nextOffset() const
{
    return m_verDef->vd_next;
}

uint32_t ElfGNUSymbolVersionDefinition::size() const
{
    if (nextOffset())
        return nextOffset();
    return section()->size() - (reinterpret_cast<const unsigned char*>(m_verDef) - section()->rawData());
}

ElfGNUSymbolVersionDefinitionAuxiliaryEntry* ElfGNUSymbolVersionDefinition::auxiliaryEntry(uint32_t index) const
{
    return m_auxEntries.at(index);
}

const unsigned char* ElfGNUSymbolVersionDefinition::rawData() const
{
    return reinterpret_cast<const unsigned char*>(m_verDef);
}
