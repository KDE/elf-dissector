/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFGNUSYMBOLVERSIONDEFINITIONAUXILIARYENTRY_H
#define ELFGNUSYMBOLVERSIONDEFINITIONAUXILIARYENTRY_H

#include <elf.h>

class ElfGNUSymbolVersionDefinition;

/** Auxiliary entries for ElfGNUSymbolVersionDefinition. */
class ElfGNUSymbolVersionDefinitionAuxiliaryEntry
{
public:
    ElfGNUSymbolVersionDefinitionAuxiliaryEntry(const ElfGNUSymbolVersionDefinitionAuxiliaryEntry&) = delete;
    explicit ElfGNUSymbolVersionDefinitionAuxiliaryEntry(ElfGNUSymbolVersionDefinition *verDef, uint32_t offset);
    ~ElfGNUSymbolVersionDefinitionAuxiliaryEntry();

    ElfGNUSymbolVersionDefinitionAuxiliaryEntry& operator=(const ElfGNUSymbolVersionDefinitionAuxiliaryEntry&) = delete;

    const ElfGNUSymbolVersionDefinition* versionDefinition() const;

    const char* name() const;
    uint32_t nextAuxiliaryEntryOffset() const;

private:
    const ElfGNUSymbolVersionDefinition *m_verDef;
    const Elf64_Verdaux *const m_auxEntry;
};

#endif // ELFGNUSYMBOLVERSIONDEFINITIONAUXILIARYENTRY_H
