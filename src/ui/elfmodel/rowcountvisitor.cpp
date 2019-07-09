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

#include "rowcountvisitor.h"

#include <elf/elffileset.h>
#include <elf/elfgnusymbolversionrequirement.h>

int RowCountVisitor::doVisit(ElfFileSet* fileSet, int) const
{
    return fileSet->size();
}

int RowCountVisitor::doVisit(ElfFile* file, int) const
{
    return file->sectionCount();
}

int RowCountVisitor::doVisit(ElfSymbolTableSection* symtab, int) const
{
    return symtab->header()->entryCount();
}

int RowCountVisitor::doVisit(ElfDynamicSection* section, int) const
{
    return section->header()->entryCount();
}

int RowCountVisitor::doVisit(ElfGNUSymbolVersionDefinitionsSection* section, int) const
{
    return section->entryCount();
}

int RowCountVisitor::doVisit(ElfGNUSymbolVersionDefinition *verDef, int) const
{
    return verDef->auxiliarySize();
}

int RowCountVisitor::doVisit(ElfGNUSymbolVersionRequirementsSection* section, int) const
{
    return section->entryCount();
}

int RowCountVisitor::doVisit(ElfGNUSymbolVersionRequirement* verNeed, int) const
{
    return verNeed->auxiliarySize();
}

int RowCountVisitor::doVisit(ElfGotSection* section, int) const
{
    return section->entryCount();
}

int RowCountVisitor::doVisit(ElfNoteSection* section, int) const
{
    return section->entryCount();
}

int RowCountVisitor::doVisit(ElfPltSection* section, int) const
{
    return section->header()->entryCount();
}

int RowCountVisitor::doVisit(ElfRelocationSection* section, int) const
{
    return section->header()->entryCount();
}

int RowCountVisitor::doVisit(DwarfInfo* info, int) const
{
    return info->compilationUnits().size();
}

int RowCountVisitor::doVisit(DwarfDie *die, int) const
{
    return die->children().size();
}
