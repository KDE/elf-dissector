/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ROWCOUNTVISITOR_H
#define ROWCOUNTVISITOR_H

#include "elfnodevisitor.h"

// TODO rename to "ChildCountVisitor"
class RowCountVisitor : public ElfNodeVisitor<int>
{
protected:
    using ElfNodeVisitor<int>::doVisit;
    int doVisit(ElfFileSet* fileSet, int arg) const override;
    int doVisit(ElfFile* file, int) const override;
    int doVisit(ElfSymbolTableSection* symtab, int) const override;
    int doVisit(ElfDynamicSection* section, int arg) const override;
    int doVisit(ElfGNUSymbolVersionDefinitionsSection* section, int arg) const override;
    int doVisit(ElfGNUSymbolVersionDefinition *verDef, int arg) const override;
    int doVisit(ElfGNUSymbolVersionRequirementsSection* section, int) const override;
    int doVisit(ElfGNUSymbolVersionRequirement *verNeed, int) const override;
    int doVisit(ElfGotSection* section, int arg) const override;
    int doVisit(ElfNoteSection* section, int arg) const override;
    int doVisit(ElfPltSection *section, int) const override;
    int doVisit(ElfRelocationSection* section, int arg) const override;
#if HAVE_DWARF
    int doVisit(DwarfInfo* info, int arg) const override;
    int doVisit(DwarfDie* die, int) const override;
#endif
};

#endif
