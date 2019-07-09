/*
    Copyright (C) 2015 Volker Krause <vkrause@kde.org>

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

#ifndef STRUCTUREPACKINGCHECK_H
#define STRUCTUREPACKINGCHECK_H

#include <QSet>

class ElfFileSet;
class DwarfInfo;
class DwarfDie;

class QString;
template<class T> class QVector;

class StructurePackingCheck
{
public:
    StructurePackingCheck() = default;
    StructurePackingCheck(const StructurePackingCheck&) = default;
    ~StructurePackingCheck() = default;

    StructurePackingCheck& operator=(const StructurePackingCheck&) = default;

    /** Set the ELF file set the checked DWARF info belongs to.*/
    void setElfFileSet(ElfFileSet *fileSet);

    void checkAll(DwarfInfo* info);
    QString checkOneStructure(DwarfDie *structDie) const;

private:
    void checkDie(DwarfDie* die);
    std::tuple<int, int> computeStructureMemoryUsage(DwarfDie* structDie, const QVector<DwarfDie*> &memberDies) const;
    QString printStructure(DwarfDie* structDie, const QVector< DwarfDie* >& memberDies) const;
    int optimalStructureSize(DwarfDie* structDie, const QVector<DwarfDie*> &memberDies) const;
    /** Look for a better type DIE for the given external one (@p typeDie). */
    DwarfDie* findTypeDefinition(DwarfDie *typeDie) const;

    ElfFileSet *m_fileSet = nullptr;
    QSet<QString> m_duplicateCheck;
};

#endif // STRUCTUREPACKINGCHECK_H
