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
