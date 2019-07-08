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

#ifndef ELFGNUSYMBOLVERSIONREQUIREMENT_H
#define ELFGNUSYMBOLVERSIONREQUIREMENT_H

#include <QVector>
#include <elf.h>

class ElfGNUSymbolVersionRequirementsSection;
class ElfGNUSymbolVersionRequirementAuxiliaryEntry;

/** Entry in the .gnu.version_r section. */
class ElfGNUSymbolVersionRequirement
{
public:
    ElfGNUSymbolVersionRequirement(const ElfGNUSymbolVersionRequirement&) = delete;
    explicit ElfGNUSymbolVersionRequirement(ElfGNUSymbolVersionRequirementsSection *section, uint32_t offset);
    ~ElfGNUSymbolVersionRequirement();

    ElfGNUSymbolVersionRequirement& operator=(const ElfGNUSymbolVersionRequirement&) = delete;

    ElfGNUSymbolVersionRequirementsSection* section() const;

    uint16_t auxiliarySize() const;
    const char* fileName() const;
    uint32_t auxiliaryOffset() const;
    uint32_t nextOffset() const;

    /** Size of this entry including its aux entries. Next one follows right afterwards. */
    uint32_t size() const;

    ElfGNUSymbolVersionRequirementAuxiliaryEntry* auxiliaryEntry(uint32_t index) const;

    const unsigned char* rawData() const;

private:
    ElfGNUSymbolVersionRequirementsSection *m_section;
    const Elf64_Verneed* const m_verNeed;
    QVector<ElfGNUSymbolVersionRequirementAuxiliaryEntry*> m_auxEntries;
};

#endif // ELFGNUSYMBOLVERSIONREQUIREMENT_H
