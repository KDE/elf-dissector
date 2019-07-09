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

#ifndef ELFGNUSYMBOLVERSIONDEFINITION_H
#define ELFGNUSYMBOLVERSIONDEFINITION_H

#include "elfgnusymbolversiondefinitionssection.h"

#include <elf.h>

class ElfGNUSymbolVersionDefinitionAuxiliaryEntry;

class ElfGNUSymbolVersionDefinition
{
public:
    ElfGNUSymbolVersionDefinition(const ElfGNUSymbolVersionDefinition&) = delete;
    explicit ElfGNUSymbolVersionDefinition(ElfGNUSymbolVersionDefinitionsSection *section, uint32_t offset);
    ~ElfGNUSymbolVersionDefinition();

    ElfGNUSymbolVersionDefinition& operator=(ElfGNUSymbolVersionDefinition&) = delete;

    ElfGNUSymbolVersionDefinitionsSection* section() const;

    uint16_t flags() const;
    uint16_t versionIndex() const;
    uint16_t auxiliarySize() const;
    uint32_t hash() const;
    uint32_t auxiliaryOffset() const;
    uint32_t nextOffset() const;

    /** Size of this entry including its aux entries. Next one follows right afterwards. */
    uint32_t size() const;

    ElfGNUSymbolVersionDefinitionAuxiliaryEntry* auxiliaryEntry(uint32_t index) const;

    const unsigned char* rawData() const;

private:
    ElfGNUSymbolVersionDefinitionsSection *m_section;
    const Elf64_Verdef* const m_verDef;
    QVector<ElfGNUSymbolVersionDefinitionAuxiliaryEntry*> m_auxEntries;
};

#endif // ELFGNUSYMBOLVERSIONDEFINITION_H
