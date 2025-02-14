/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef INDEXVISITOR_H
#define INDEXVISITOR_H

#include "elfnodevisitor.h"

#include <QPair>

// TODO: better name "ChildVisitor"?
class IndexVisitor : public ElfNodeVisitor<QPair<void*, ElfNodeVariant::Type>>
{
public:
    using ElfNodeVisitor<QPair<void*, ElfNodeVariant::Type>>::doVisit;
    type doVisit(ElfFileSet* fileSet, int row) const override;
    type doVisit(ElfFile* file, int row) const override;
    type doVisit(ElfSymbolTableSection* symtab, int row) const override;
    type doVisit(ElfDynamicSection* section, int row) const override;
    type doVisit(ElfGNUSymbolVersionDefinitionsSection *section, int row) const override;
    type doVisit(ElfGNUSymbolVersionDefinition *verDef, int row) const override;
    type doVisit(ElfGNUSymbolVersionRequirementsSection* section, int arg) const override;
    type doVisit(ElfGNUSymbolVersionRequirement *verNeed, int row) const override;
    type doVisit(ElfGotSection *section, int row) const override;
    type doVisit(ElfNoteSection* section, int row) const override;
    type doVisit(ElfPltSection *section, int row) const override;
    type doVisit(ElfRelocationSection *section, int row) const override;
#if HAVE_DWARF
    type doVisit(DwarfInfo* info, int row) const override;
    type doVisit(DwarfDie* die, int row) const override;
#endif
};

#endif // INDEXVISITOR_H
