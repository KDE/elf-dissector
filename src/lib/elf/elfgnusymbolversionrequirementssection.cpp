/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "elfgnusymbolversionrequirementssection.h"
#include "elfgnusymbolversionrequirement.h"
#include "elfgnusymbolversionrequirementauxiliaryentry.h"
#include "elfdynamicsection.h"
#include "elffile.h"

ElfGNUSymbolVersionRequirementsSection::ElfGNUSymbolVersionRequirementsSection(ElfFile* file, ElfSectionHeader* shdr) :
    ElfSection(file, shdr)
{
}

ElfGNUSymbolVersionRequirementsSection::~ElfGNUSymbolVersionRequirementsSection()
{
    qDeleteAll(m_versionRequirements);
}

uint32_t ElfGNUSymbolVersionRequirementsSection::entryCount() const
{
    return m_versionRequirements.size();
}

ElfGNUSymbolVersionRequirement* ElfGNUSymbolVersionRequirementsSection::requirement(uint32_t index) const
{
    return m_versionRequirements.at(index);
}

ElfGNUSymbolVersionRequirementAuxiliaryEntry* ElfGNUSymbolVersionRequirementsSection::requirementForVersionIndex(uint16_t index) const
{
    for (uint32_t i = 0; i < entryCount(); ++i) {
        auto req = m_versionRequirements.at(i);
        for (uint16_t j = 0; j < req->auxiliarySize(); ++j) {
            auto reqAux = req->auxiliaryEntry(j);
            if ((reqAux->other() & 0x7FFF) == (index & 0x7FFF))
                return reqAux;
        }
    }
    return nullptr;
}

void ElfGNUSymbolVersionRequirementsSection::parse()
{
    // TODO parse until nextOffset() is 0 might be an alternative, removes dependency on dynamicSection() being available here
    const auto verNeedNum = file()->dynamicSection()->entryWithTag(DT_VERNEEDNUM);
    if (!verNeedNum)
        return;

    uint32_t offset = 0;
    m_versionRequirements.reserve(verNeedNum->value());
    for (uint i = 0; i < verNeedNum->value(); ++i) {
        const auto entry = new ElfGNUSymbolVersionRequirement(this, offset);
        m_versionRequirements.push_back(entry);
        offset += entry->nextOffset();
    }
}
