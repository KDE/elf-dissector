/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFGNUSYMBOLVERSIONDEFINITIONSSECTION_H
#define ELFGNUSYMBOLVERSIONDEFINITIONSSECTION_H

#include "elfsection.h"

#include <QList>

class ElfGNUSymbolVersionDefinition;

/** .gnu.version_d section. */
class ElfGNUSymbolVersionDefinitionsSection : public ElfSection
{
public:
    explicit ElfGNUSymbolVersionDefinitionsSection(ElfFile *file, ElfSectionHeader *shdr);
    ~ElfGNUSymbolVersionDefinitionsSection();

    uint32_t entryCount() const;
    /** This returns the definition at position @p index in the file.
     *  Ie. @p index is a 0-based array index, unlike in the method below.
     */
    ElfGNUSymbolVersionDefinition* definition(uint32_t index) const;
    /** This returns the definition with version index @p index.
     *  Ie. @p index is the number referenced in the various version entries.
     */
    ElfGNUSymbolVersionDefinition* definitionForVersionIndex(uint16_t index) const;

protected:
    // parsing needs to be delayed until file()->dynamicSection() works
    friend class ElfFile;
    void parse();

private:
     QList<ElfGNUSymbolVersionDefinition*> m_versionDefinitions;
};

#endif // ELFGNUSYMBOLVERSIONDEFINITIONSSECTION_H
