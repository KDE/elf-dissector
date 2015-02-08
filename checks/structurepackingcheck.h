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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef STRUCTUREPACKINGCHECK_H
#define STRUCTUREPACKINGCHECK_H

#include <QSet>

class DwarfInfo;
class DwarfDie;

class QString;
template<class T> class QVector;

class StructurePackingCheck
{
public:
    void checkAll(DwarfInfo* info);
    void checkDie(DwarfDie* die);

private:
    void checkStructure(DwarfDie* structDie, const QVector<DwarfDie*> &memberDies);
    QString printStructure(DwarfDie* structDie, const QVector< DwarfDie* >& memberDies);
    int optimalStructureSize(DwarfDie* structDie, const QVector<DwarfDie*> &memberDies);

    QSet<QString> m_duplicateCheck;
};

#endif // STRUCTUREPACKINGCHECK_H
