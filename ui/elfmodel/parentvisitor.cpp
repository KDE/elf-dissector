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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "parentvisitor.h"
#include <elf/elffileset.h>
#include <elf/elfgnusymbolversiondefinitionauxiliaryentry.h>
#include <elf/elfgnusymbolversionrequirement.h>

#include <QDebug>

#include <elf.h>

#include <cassert>

ParentVisitor::ParentVisitor(ElfFileSet* parent) : m_fileSet(parent)
{
}

QPair<void*, int> ParentVisitor::doVisit(ElfFile* file, int) const
{
    int row = 0;
    for (; row < m_fileSet->size(); ++row) {
        if (m_fileSet->file(row) == file)
            break;
    }
    return qMakePair<void*, int>(m_fileSet, row);
}

QPair<void*, int> ParentVisitor::doVisit(ElfSection* section, int) const
{
    return qMakePair<void*, int>(section->file(), section->header()->sectionIndex());
}

QPair< void*, int > ParentVisitor::doVisit(ElfGNUSymbolVersionDefinition* verDef, int) const
{
    int row = -1;
    for (uint i = 0; i < verDef->section()->entryCount(); ++i) {
        if (verDef->section()->definition(i) == verDef) {
            row = i;
            break;
        }
    }
    assert(row >= 0);
    return qMakePair<void*, int>(verDef->section(), row);
}

QPair< void*, int > ParentVisitor::doVisit(ElfGNUSymbolVersionRequirement *verNeed, int) const
{
    int row = -1;
    for (uint i = 0; i < verNeed->section()->entryCount(); ++i) {
        if (verNeed->section()->requirement(i) == verNeed) {
            row = i;
            break;
        }
    }
    assert(row >= 0);
    return qMakePair<void*, int>(verNeed->section(), row);
}

QPair< void*, int > ParentVisitor::doVisit(DwarfInfo* info, int) const
{
    return qMakePair<void*, int>(info->elfFile(), info->elfFile()->indexOfSection(".debug_info"));
}

QPair< void*, int > ParentVisitor::doVisit(DwarfDie* die, int) const
{
    if (die->parentDie()) {
        return qMakePair<void*, int>(die->parentDie(), die->parentDie()->children().indexOf(die));
    }
    return qMakePair<void*, int>(die->dwarfInfo(), die->dwarfInfo()->compilationUnits().indexOf(die));
}
