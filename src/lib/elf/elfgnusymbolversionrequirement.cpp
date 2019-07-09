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

#include "elfgnusymbolversionrequirement.h"
#include "elfgnusymbolversionrequirementssection.h"
#include "elfgnusymbolversionrequirementauxiliaryentry.h"
#include "elfstringtablesection.h"

#include <cassert>

ElfGNUSymbolVersionRequirement::ElfGNUSymbolVersionRequirement(ElfGNUSymbolVersionRequirementsSection* section, uint32_t offset) :
    m_section(section),
    m_verNeed(reinterpret_cast<const Elf64_Verneed*>(section->rawData() + offset))
{
    // 32bit and 64bit have exactly the same memory layout
    static_assert(sizeof(Elf32_Verneed) == sizeof(Elf64_Verneed), "SHT_GNU_verneed memory layout changed");

    assert(m_verNeed->vn_version == 1);
    static_assert(VER_NEED_CURRENT == 1, "SHT_GNU_verneed format changed!");

    uint32_t auxOffset = 0;
    m_auxEntries.reserve(auxiliarySize());
    for (int i = 0; i < auxiliarySize(); ++i) {
        const auto auxEntry = new ElfGNUSymbolVersionRequirementAuxiliaryEntry(this, auxOffset);
        m_auxEntries.push_back(auxEntry);
        auxOffset += auxEntry->nextAuxiliaryEntryOffset();
    }

    assert(auxiliarySize() == m_auxEntries.size());
}

ElfGNUSymbolVersionRequirement::~ElfGNUSymbolVersionRequirement()
{
    qDeleteAll(m_auxEntries);
}

ElfGNUSymbolVersionRequirementsSection* ElfGNUSymbolVersionRequirement::section() const
{
    return m_section;
}

uint16_t ElfGNUSymbolVersionRequirement::auxiliarySize() const
{
    return m_verNeed->vn_cnt;
}

const char* ElfGNUSymbolVersionRequirement::fileName() const
{
    return section()->linkedSection<ElfStringTableSection>()->string(m_verNeed->vn_file);
}

uint32_t ElfGNUSymbolVersionRequirement::auxiliaryOffset() const
{
    return m_verNeed->vn_aux;
}

uint32_t ElfGNUSymbolVersionRequirement::nextOffset() const
{
    return m_verNeed->vn_next;
}

ElfGNUSymbolVersionRequirementAuxiliaryEntry* ElfGNUSymbolVersionRequirement::auxiliaryEntry(uint32_t index) const
{
    return m_auxEntries.at(index);
}

uint32_t ElfGNUSymbolVersionRequirement::size() const
{
    if (nextOffset())
        return nextOffset();
    return section()->size() - (reinterpret_cast<const unsigned char*>(m_verNeed) - section()->rawData());
}

const unsigned char* ElfGNUSymbolVersionRequirement::rawData() const
{
    return reinterpret_cast<const unsigned char*>(m_verNeed);
}
