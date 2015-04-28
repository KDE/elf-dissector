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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ELFGNUSYMBOLVERSIONDEFINITION_H
#define ELFGNUSYMBOLVERSIONDEFINITION_H

#include "elfgnusymbolversiondefinitionssection.h"

#include <elf.h>

class ElfGNUSymbolVersionDefinition
{
public:
    explicit ElfGNUSymbolVersionDefinition(ElfGNUSymbolVersionDefinitionsSection *section, uint32_t offset);
    ~ElfGNUSymbolVersionDefinition();

    ElfGNUSymbolVersionDefinitionsSection* section() const;

    uint16_t flags() const;
    uint16_t versionIndex() const;
    uint16_t auxSize() const;
    uint32_t hash() const;
    uint32_t auxOffset() const;
    uint32_t nextOffset() const;

    /** Size of this entry including its aux entries. Next one follows right afterwards. */
    uint32_t size() const;

private:
    ElfGNUSymbolVersionDefinitionsSection *m_section;
    const Elf64_Verdef* const m_verDef;
};

#endif // ELFGNUSYMBOLVERSIONDEFINITION_H
