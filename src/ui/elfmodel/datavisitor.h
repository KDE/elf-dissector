/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
#if HAVE_DWARF
    QVariant doVisit(DwarfInfo *info, int arg) const override;
    QVariant doVisit(DwarfDie *die, int arg) const override;
#endif

private:
    friend class NavigatingDisassembler;
    QString printSectionName(ElfSection *section) const;
    QString printSymbolName(ElfSymbolTableEntry *symbol) const;
    QString printRelocation(ElfRelocationEntry *entry) const;
#if HAVE_DWARF
    QString printDwarfDie(DwarfDie* die) const;
    QString printDwarfDieName(DwarfDie* die) const;
#endif

private:
    const ElfModel* const m_model;
    int m_column;
};

#endif // DATAVISITOR_H
