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

#include "parentvisitor.h"
#include "elfmodel.h"

#include <elf/elffileset.h>
#include <elf/elfgnusymbolversiondefinitionauxiliaryentry.h>
#include <elf/elfgnusymbolversionrequirement.h>
#include <dwarf/dwarfcudie.h>


#include <elf.h>

#include <cassert>

ParentVisitor::ParentVisitor(const ElfModel* model) : m_model(model)
{
}

ParentVisitor::type ParentVisitor::doVisit(ElfFile* file, int) const
{
    int row = 0;
    for (; row < m_model->fileSet()->size(); ++row) {
        if (m_model->fileSet()->file(row) == file)
            break;
    }
    return makeParent(m_model->fileSet(), ElfNodeVariant::FileSet, row);
}

ParentVisitor::type ParentVisitor::doVisit(ElfSection* section, int) const
{
    auto file = section->file();
    if (file->isSeparateDebugFile())
        file = file->contentFile();
    return makeParent(file, ElfNodeVariant::File, section->header()->sectionIndex());
}

ParentVisitor::type ParentVisitor::doVisit(ElfGNUSymbolVersionDefinition* verDef, int) const
{
    int row = -1;
    for (uint i = 0; i < verDef->section()->entryCount(); ++i) {
        if (verDef->section()->definition(i) == verDef) {
            row = i;
            break;
        }
    }
    assert(row >= 0);
    return makeParent(verDef->section(), ElfNodeVariant::VersionDefinitionSection, row);
}

ParentVisitor::type ParentVisitor::doVisit(ElfGNUSymbolVersionRequirement *verNeed, int) const
{
    int row = -1;
    for (uint i = 0; i < verNeed->section()->entryCount(); ++i) {
        if (verNeed->section()->requirement(i) == verNeed) {
            row = i;
            break;
        }
    }
    assert(row >= 0);
    return makeParent(verNeed->section(), ElfNodeVariant::VersionRequirementsSection, row);
}

ParentVisitor::type ParentVisitor::doVisit(ElfGotEntry* entry, int) const
{
    return makeParent(entry->section(), ElfNodeVariant::GotSection, entry->index());
}

ParentVisitor::type ParentVisitor::doVisit(ElfPltEntry* entry, int) const
{
    return makeParent(entry->section(), ElfNodeVariant::PltSection, entry->index());
}

ParentVisitor::type ParentVisitor::doVisit(ElfSymbolTableEntry *symbol, int) const
{
    return makeParent(const_cast<ElfSymbolTableSection*>(symbol->symbolTable()), ElfNodeVariant::SymbolTableSection, symbol->index());
}

ParentVisitor::type ParentVisitor::doVisit(DwarfInfo* info, int) const
{
    return makeParent(info->elfFile(), ElfNodeVariant::File, info->elfFile()->indexOfSection(".debug_info"));
}

ParentVisitor::type ParentVisitor::doVisit(DwarfDie* die, int) const
{
    if (die->parentDie()) {
        return makeParent(die->parentDie(), ElfNodeVariant::DwarfDie, die->parentDie()->children().indexOf(die));
    }
    return makeParent(die->dwarfInfo(), ElfNodeVariant::DwarfInfo, die->dwarfInfo()->compilationUnits().indexOf(static_cast<DwarfCuDie*>(die)));
}

ParentVisitor::type ParentVisitor::makeParent(void* payload, ElfNodeVariant::Type type, int row) const
{
    return qMakePair(m_model->makeVariant(payload, type), row);
}
