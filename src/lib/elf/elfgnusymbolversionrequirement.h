/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFGNUSYMBOLVERSIONREQUIREMENT_H
#define ELFGNUSYMBOLVERSIONREQUIREMENT_H

#include <QList>
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
    QList<ElfGNUSymbolVersionRequirementAuxiliaryEntry*> m_auxEntries;
};

#endif // ELFGNUSYMBOLVERSIONREQUIREMENT_H
