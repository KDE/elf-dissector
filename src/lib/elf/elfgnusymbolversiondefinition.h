/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
    QList<ElfGNUSymbolVersionDefinitionAuxiliaryEntry*> m_auxEntries;
};

#endif // ELFGNUSYMBOLVERSIONDEFINITION_H
