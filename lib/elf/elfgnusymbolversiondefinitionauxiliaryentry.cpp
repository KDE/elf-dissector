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
