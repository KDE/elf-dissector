/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PARENTVISITOR_H
#define PARENTVISITOR_H

#include "elfnodevisitor.h"

#include <QPair>

class ElfModel;

class ParentVisitor : public ElfNodeVisitor<QPair<ElfNodeVariant*, int>>
{
public:
    explicit ParentVisitor(const ElfModel* model);

protected:
    using ElfNodeVisitor<QPair<ElfNodeVariant*, int>>::doVisit;
    type doVisit(ElfFile* file, int) const override;
    type doVisit(ElfSection* section, int) const override;
    type doVisit(ElfGNUSymbolVersionDefinition* verDef, int) const override;
    type doVisit(ElfGNUSymbolVersionRequirement *verNeed, int) const override;
    type doVisit(ElfGotEntry *entry, int) const override;
    type doVisit(ElfPltEntry *entry, int) const override;
    type doVisit(ElfSymbolTableEntry *symbold, int) const override;
#if HAVE_DWARF
    type doVisit(DwarfInfo* info, int) const override;
    type doVisit(DwarfDie* die, int) const override;
#endif

private:
    type makeParent(void* payload, ElfNodeVariant::Type type, int row) const;

    const ElfModel* const m_model;
};

#endif // PARENTVISITOR_H
