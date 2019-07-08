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

#ifndef ELFGNUSYMBOLVERSIONREQUIREMENTAUXILIARYENTRY_H
#define ELFGNUSYMBOLVERSIONREQUIREMENTAUXILIARYENTRY_H

#include <elf.h>

class ElfGNUSymbolVersionRequirement;

/** Auxiliary entries for ElfGNUSymbolVersionRequirement. */
class ElfGNUSymbolVersionRequirementAuxiliaryEntry
{
public:
    ElfGNUSymbolVersionRequirementAuxiliaryEntry(const ElfGNUSymbolVersionRequirementAuxiliaryEntry&) = delete;
    explicit ElfGNUSymbolVersionRequirementAuxiliaryEntry(ElfGNUSymbolVersionRequirement *verNeed, uint32_t offset);
    ~ElfGNUSymbolVersionRequirementAuxiliaryEntry();

    ElfGNUSymbolVersionRequirementAuxiliaryEntry& operator=(const ElfGNUSymbolVersionRequirementAuxiliaryEntry&) = delete;

    const ElfGNUSymbolVersionRequirement* versionRequirement() const;

    uint32_t hash() const;
    uint16_t flags() const;
    uint16_t other() const;
    const char* name() const;
    uint32_t nextAuxiliaryEntryOffset() const;

private:
    const ElfGNUSymbolVersionRequirement *m_verNeed;
    const Elf64_Vernaux *const m_auxEntry;
};

#endif // ELFGNUSYMBOLVERSIONREQUIREMENTAUXILIARYENTRY_H
