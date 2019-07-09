/*
    Copyright (C) 2013-2014 Volker Krause <vkrause@kde.org>

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
    int doVisit(DwarfInfo* info, int arg) const override;
    int doVisit(DwarfDie* die, int) const override;
};

#endif
