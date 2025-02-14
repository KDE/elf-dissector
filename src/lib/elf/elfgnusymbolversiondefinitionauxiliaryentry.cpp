/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "elfgnusymbolversiondefinitionauxiliaryentry.h"
#include "elfgnusymbolversiondefinition.h"
#include "elfstringtablesection.h"
#include "elffile.h"

ElfGNUSymbolVersionDefinitionAuxiliaryEntry::ElfGNUSymbolVersionDefinitionAuxiliaryEntry(ElfGNUSymbolVersionDefinition* verDef, uint32_t offset) :
    m_verDef(verDef),
    m_auxEntry(reinterpret_cast<const Elf64_Verdaux*>(verDef->rawData() + verDef->auxiliaryOffset() + offset))
{
    static_assert(sizeof(Elf32_Verdaux) == sizeof(Elf64_Verdaux), "SHT_GNU_verdef auxiliary entry layout changed");
}

ElfGNUSymbolVersionDefinitionAuxiliaryEntry::~ElfGNUSymbolVersionDefinitionAuxiliaryEntry() = default;

const ElfGNUSymbolVersionDefinition* ElfGNUSymbolVersionDefinitionAuxiliaryEntry::versionDefinition() const
{
    return m_verDef;
}

const char* ElfGNUSymbolVersionDefinitionAuxiliaryEntry::name() const
{
    return versionDefinition()->section()->linkedSection<ElfStringTableSection>()->string(m_auxEntry->vda_name);
}

uint32_t ElfGNUSymbolVersionDefinitionAuxiliaryEntry::nextAuxiliaryEntryOffset() const
{
    return m_auxEntry->vda_next;
}
