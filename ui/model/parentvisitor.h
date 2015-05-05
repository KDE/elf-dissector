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

#ifndef PARENTVISITOR_H
#define PARENTVISITOR_H

#include "elfnodevisitor.h"

#include <QPair>

class ParentVisitor : public ElfNodeVisitor<QPair<void*, int>>
{
public:
    explicit ParentVisitor(ElfFileSet* parent);

protected:
    type doVisit(ElfFile* file, int) const override;
    type doVisit(ElfSection* section, int) const override;
    type doVisit(ElfGNUSymbolVersionDefinition* verDef, int) const override;
    type doVisit(ElfGNUSymbolVersionRequirement *verNeed, int) const override;
    type doVisit(DwarfInfo* info, int) const override;
    type doVisit(DwarfDie* die, int) const override;

private:
    ElfFileSet *m_fileSet;
};

#endif // PARENTVISITOR_H
