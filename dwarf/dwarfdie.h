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

#ifndef DWARFDIE_H
#define DWARFDIE_H

#include <QPair>
#include <QVariant>
#include <QVector>

#include <libdwarf/libdwarf.h>

class DwarfInfo;
class QString;

class DwarfDie
{
public:
    ~DwarfDie(); // TODO dealloc m_die

    DwarfInfo* dwarfInfo() const;
    DwarfDie* parentDIE() const;

    QString name() const;
    Dwarf_Half tag() const;
    const char* tagName() const;
    Dwarf_Off offset() const;
    QVector<QPair<QString, QVariant>> attributes() const;

    QVector<DwarfDie*> children() const;
    DwarfDie* dieAtOffset(Dwarf_Off offset) const;

private:
    friend class DwarfInfoPrivate;
    DwarfDie(Dwarf_Die die, DwarfDie* parent);
    DwarfDie(Dwarf_Die die, DwarfInfo* info);

    void scanChildren() const;

    const char* sourceFileForIndex(int i) const;

    Dwarf_Debug dwarfHandle() const;

    Dwarf_Die m_die = nullptr;
    DwarfDie *m_parent = nullptr;
    DwarfInfo *m_info = nullptr;

    mutable QVector<DwarfDie*> m_children;
    mutable char** m_srcFiles = nullptr;
    mutable Dwarf_Signed m_srcFileCount = 0;

    mutable bool m_childrenScanned = false;
};

#endif // DWARFDIE_H
