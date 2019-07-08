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

#ifndef DWARFDIE_H
#define DWARFDIE_H

#include <QVariant>
#include <QVector>

#include <libdwarf.h>

class DwarfInfo;
class DwarfCuDie;
class QString;

class DwarfDie
{
public:
    DwarfDie(const DwarfDie&) = delete;
    ~DwarfDie();

    DwarfDie& operator=(const DwarfDie&) = delete;

    DwarfInfo* dwarfInfo() const;
    DwarfDie* parentDie() const;
    bool isCompilationUnit() const;

    /** Content of the name attribute. */
    QByteArray name() const;
    Dwarf_Half tag() const;
    QByteArray tagName() const;
    Dwarf_Off offset() const;

    /** If this DIE represents a type, this is the full type name. */
    QByteArray typeName() const;
    /** If this DIE represents a type, this is the size occupied by this type in bytes. */
    int typeSize() const;
    /** If this DIE represents a type, this returns the alignment needed for it. */
    int typeAlignment() const;

    /** If this is a DW_TAG_member, check if this is a static member, or a non-static one. */
    bool isStaticMember() const;

    /** Best effort human readable distplay string. */
    QString displayName() const;
    /** Fully qualified name (including class/namespaces etc). */
    QByteArray fullyQualifiedName() const;
    /** Path to the source file. Best effort attempt to make it absolute, but that can't be guaranteed. */
    QString sourceFilePath() const;
    /** Source code location, best effort to find an absolute path, and line number if present. */
    QString sourceLocation() const;

    QVector<Dwarf_Half> attributes() const;
    static QByteArray attributeName(Dwarf_Half attributeType);
    QVariant attribute(Dwarf_Half attributeType) const;

    QVector<DwarfDie*> children() const;
    DwarfDie* dieAtOffset(Dwarf_Off offset) const;

    /** If this DIE is inheriting attributes from another DIE, that's returned here. */
    DwarfDie* inheritedFrom() const;

    // internal
    const DwarfCuDie* compilationUnit() const;
    Dwarf_Die dieHandle() const;

protected:
    friend class DwarfInfoPrivate;
    DwarfDie(Dwarf_Die die, DwarfDie* parent);
    DwarfDie(Dwarf_Die die, DwarfInfo* info);

    QVariant attributeLocal(Dwarf_Half attributeType) const;

    void scanChildren() const;

    Dwarf_Debug dwarfHandle() const;

    Dwarf_Die m_die = nullptr;
    union {
        DwarfDie *parent = nullptr;
        DwarfInfo *info;
    } m_parent;

    mutable QVector<DwarfDie*> m_children;
    mutable bool m_childrenScanned = false;
};

Q_DECLARE_METATYPE(DwarfDie*)

#endif // DWARFDIE_H
