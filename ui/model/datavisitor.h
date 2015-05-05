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

#ifndef DATAVISITOR_H
#define DATAVISITOR_H

#include "elfnodevisitor.h"

#include <QVariant>

class DataVisitor : public ElfNodeVisitor<QVariant>
{
public:
    explicit DataVisitor(ElfFileSet* fileSet);

protected:
    QVariant doVisit(ElfFile* file, int arg) const override;
    QVariant doVisit(ElfSection* section, int arg) const override;
    QVariant doVisit(ElfSymbolTableSection* symtab, int arg) const override;
    QVariant doVisit(ElfSymbolTableEntry* entry, int arg) const override;
    QVariant doVisit(ElfDynamicEntry* entry, int arg) const override;
    QVariant doVisit(ElfGNUSymbolVersionDefinition *verDef, int role) const override;
    QVariant doVisit(ElfGNUSymbolVersionDefinitionAuxiliaryEntry *auxEntry, int role) const override;
    QVariant doVisit(ElfGNUSymbolVersionRequirement *verNeed, int role) const override;
    QVariant doVisit(ElfGNUSymbolVersionRequirementAuxiliaryEntry *auxEntry, int role) const override;
    QVariant doVisit(ElfNoteEntry *entry, int role) const override;
    QVariant doVisit(ElfRelocationEntry *entry, int arg) const override;
    QVariant doVisit(DwarfInfo *info, int arg) const override;
    QVariant doVisit(DwarfDie *die, int arg) const override;

private:
    ElfFileSet *m_fileSet;
};

#endif // DATAVISITOR_H
