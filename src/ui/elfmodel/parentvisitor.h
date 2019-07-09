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
    type doVisit(DwarfInfo* info, int) const override;
    type doVisit(DwarfDie* die, int) const override;

private:
    type makeParent(void* payload, ElfNodeVariant::Type type, int row) const;

    const ElfModel* const m_model;
};

#endif // PARENTVISITOR_H
