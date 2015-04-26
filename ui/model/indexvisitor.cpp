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

#include "indexvisitor.h"

#include <elf/elffile.h>
#include <elf/elffileset.h>

#include <elf.h>

IndexVisitor::type IndexVisitor::doVisit(ElfFileSet* fileSet, int row) const
{
    return qMakePair<void*, ElfNodeVariant::Type>(fileSet->file(row), ElfNodeVariant::File);
}

IndexVisitor::type IndexVisitor::doVisit(ElfFile* file, int row) const
{
    ElfSection *section = file->section<ElfSection>(row);
    void *internalPointer = section;
    ElfNodeVariant::Type type;
    switch (section->header()->type()) {
        case SHT_SYMTAB:
        case SHT_DYNSYM:
            type = ElfNodeVariant::SymbolTableSection;
            break;
        case SHT_DYNAMIC:
            type = ElfNodeVariant::DynamicSection;
            break;
        case SHT_NOTE:
            type = ElfNodeVariant::NoteSection;
            break;
        case SHT_REL:
        case SHT_RELA:
            type = ElfNodeVariant::RelocationSection;
            break;
        default:
            if (qstrcmp(section->header()->name(), ".debug_info") == 0) {
                type = ElfNodeVariant::DwarfInfo;
                internalPointer = file->dwarfInfo();
            } else {
                type = ElfNodeVariant::Section;
            }
    }
    return qMakePair(internalPointer, type);
}

IndexVisitor::type IndexVisitor::doVisit(ElfSymbolTableSection* symtab, int row) const
{
    const auto entry = symtab->entry(row);
    return qMakePair<void*, ElfNodeVariant::Type>(entry, ElfNodeVariant::SymbolTableEntry);
}

IndexVisitor::type IndexVisitor::doVisit(ElfDynamicSection* section, int row) const
{
    const auto entry = section->entry(row);
    return qMakePair<void*, ElfNodeVariant::Type>(entry, ElfNodeVariant::DynamicEntry);
}

IndexVisitor::type IndexVisitor::doVisit(ElfNoteSection *section, int row) const
{
    const auto entry = section->entry(row);
    return qMakePair<void*, ElfNodeVariant::Type>(entry, ElfNodeVariant::NoteEntry);
}

IndexVisitor::type IndexVisitor::doVisit(ElfRelocationSection* section, int row) const
{
    const auto entry = section->entry(row);
    return qMakePair<void*, ElfNodeVariant::Type>(entry, ElfNodeVariant::RelocationEntry);
}

IndexVisitor::type IndexVisitor::doVisit(DwarfInfo* info, int row) const
{
    DwarfDie *cuDie = info->compilationUnits().at(row);
    return qMakePair<void*, ElfNodeVariant::Type>(cuDie, ElfNodeVariant::DwarfDie);
}

QPair< void*, ElfNodeVariant::Type > IndexVisitor::doVisit(DwarfDie* die, int row) const
{
    DwarfDie *childDie = die->children().at(row);
    return qMakePair<void*, ElfNodeVariant::Type>(childDie, ElfNodeVariant::DwarfDie);
}
