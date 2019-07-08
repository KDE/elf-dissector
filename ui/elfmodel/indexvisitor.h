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
    type doVisit(DwarfInfo* info, int row) const override;
    type doVisit(DwarfDie* die, int row) const override;
};

#endif // INDEXVISITOR_H
