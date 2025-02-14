/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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

#if HAVE_DWARF
int RowCountVisitor::doVisit(DwarfInfo* info, int) const
{
    return info->compilationUnits().size();
}

int RowCountVisitor::doVisit(DwarfDie *die, int) const
{
    return die->children().size();
}
#endif
