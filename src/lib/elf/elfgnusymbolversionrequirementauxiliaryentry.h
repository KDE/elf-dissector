/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
