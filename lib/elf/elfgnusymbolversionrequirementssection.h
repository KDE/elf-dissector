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

#ifndef ELFGNUSYMBOLVERSIONREQUIREMENTSSECTION_H
#define ELFGNUSYMBOLVERSIONREQUIREMENTSSECTION_H

#include "elfsection.h"

#include <QVector>

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
    QVector<ElfGNUSymbolVersionRequirement*> m_versionRequirements;

};

#endif // ELFGNUSYMBOLVERSIONREQUIREMENTSSECTION_H
