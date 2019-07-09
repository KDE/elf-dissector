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

#include "elfgnusymbolversionrequirementauxiliaryentry.h"
#include "elfgnusymbolversionrequirement.h"
#include "elfgnusymbolversionrequirementssection.h"
#include "elfstringtablesection.h"

ElfGNUSymbolVersionRequirementAuxiliaryEntry::ElfGNUSymbolVersionRequirementAuxiliaryEntry(ElfGNUSymbolVersionRequirement* verNeed, uint32_t offset) :
    m_verNeed(verNeed),
    m_auxEntry(reinterpret_cast<const Elf64_Vernaux*>(verNeed->rawData() + verNeed->auxiliaryOffset() + offset))
{
    static_assert(sizeof(Elf32_Vernaux) == sizeof(Elf64_Vernaux), "SHT_GNU_verneed auxiliary entry layout changed");
}

ElfGNUSymbolVersionRequirementAuxiliaryEntry::~ElfGNUSymbolVersionRequirementAuxiliaryEntry() = default;

const ElfGNUSymbolVersionRequirement* ElfGNUSymbolVersionRequirementAuxiliaryEntry::versionRequirement() const
{
    return m_verNeed;
}

uint32_t ElfGNUSymbolVersionRequirementAuxiliaryEntry::hash() const
{
    return m_auxEntry->vna_hash;
}

uint16_t ElfGNUSymbolVersionRequirementAuxiliaryEntry::flags() const
{
    return m_auxEntry->vna_flags;
}

uint16_t ElfGNUSymbolVersionRequirementAuxiliaryEntry::other() const
{
    return m_auxEntry->vna_other;
}

const char* ElfGNUSymbolVersionRequirementAuxiliaryEntry::name() const
{
    return m_verNeed->section()->linkedSection<ElfStringTableSection>()->string(m_auxEntry->vna_name);
}

uint32_t ElfGNUSymbolVersionRequirementAuxiliaryEntry::nextAuxiliaryEntryOffset() const
{
    return m_auxEntry->vna_next;
}
