/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFGNUSYMBOLVERSIONREQUIREMENTSSECTION_H
#define ELFGNUSYMBOLVERSIONREQUIREMENTSSECTION_H

#include "elfsection.h"

#include <QList>

class ElfGNUSymbolVersionRequirement;
class ElfGNUSymbolVersionRequirementAuxiliaryEntry;

/** Represents the .gnu.version_r section. */
class ElfGNUSymbolVersionRequirementsSection : public ElfSection
{
public:
    explicit ElfGNUSymbolVersionRequirementsSection(ElfFile* file, ElfSectionHeader* shdr);
    ~ElfGNUSymbolVersionRequirementsSection();

    uint32_t entryCount() const;
    /** This returns the requirement at position @p index in the file.
     *  Ie. @p index is a 0-based array index, unlike in the method below.
     */
    ElfGNUSymbolVersionRequirement* requirement(uint32_t index) const;
    /** This returns the requirement with version index @p index.
     *  Ie. @p index is the number referenced in the various version entries.
     */
    ElfGNUSymbolVersionRequirementAuxiliaryEntry* requirementForVersionIndex(uint16_t index) const;

protected:
    // parsing needs to be delayed until file()->dynamicSection() works
    friend class ElfFile;
    void parse();

private:
    QList<ElfGNUSymbolVersionRequirement*> m_versionRequirements;

};

#endif // ELFGNUSYMBOLVERSIONREQUIREMENTSSECTION_H
