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

#ifndef DATAVISITOR_H
#define DATAVISITOR_H

#include "elfnodevisitor.h"

#include <QVariant>

class ElfModel;

class DataVisitor : public ElfNodeVisitor<QVariant>
{
public:
    explicit DataVisitor(const ElfModel* model, int column);

protected:
    using ElfNodeVisitor<QVariant>::doVisit;
    QVariant doVisit(ElfFile* file, int arg) const override;
    QVariant doVisit(ElfSection* section, int arg) const override;
    QVariant doVisit(ElfSymbolTableSection* symtab, int arg) const override;
    QVariant doVisit(ElfSymbolTableEntry* entry, int arg) const override;
    QVariant doVisit(ElfGnuDebugLinkSection *section, int role) const override;
    QVariant doVisit(ElfDynamicEntry* entry, int arg) const override;
    QVariant doVisit(ElfHashSection* section, int role) const override;
    QVariant doVisit(ElfGNUSymbolVersionDefinition *verDef, int role) const override;
    QVariant doVisit(ElfGNUSymbolVersionDefinitionAuxiliaryEntry *auxEntry, int role) const override;
    QVariant doVisit(ElfGNUSymbolVersionRequirement *verNeed, int role) const override;
    QVariant doVisit(ElfGNUSymbolVersionRequirementAuxiliaryEntry *auxEntry, int role) const override;
    QVariant doVisit(ElfGotEntry *entry, int role) const override;
    QVariant doVisit(ElfNoteEntry *entry, int role) const override;
    QVariant doVisit(ElfPltEntry *entry, int role) const override;
    QVariant doVisit(ElfRelocationEntry *entry, int arg) const override;
    QVariant doVisit(DwarfInfo *info, int arg) const override;
    QVariant doVisit(DwarfDie *die, int arg) const override;

private:
    friend class NavigatingDisassembler;
    QString printSectionName(ElfSection *section) const;
    QString printSymbolName(ElfSymbolTableEntry *symbol) const;
    QString printRelocation(ElfRelocationEntry *entry) const;
    QString printDwarfDie(DwarfDie* die) const;
    QString printDwarfDieName(DwarfDie* die) const;

private:
    const ElfModel* const m_model;
    int m_column;
};

#endif // DATAVISITOR_H
