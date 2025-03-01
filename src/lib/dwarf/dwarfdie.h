/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DWARFDIE_H
#define DWARFDIE_H

#include <QVariant>
#include <QList>

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
    Dwarf_Half version() const;

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

    QList<Dwarf_Half> attributes() const;
    static QByteArray attributeName(Dwarf_Half attributeType);
    QVariant attribute(Dwarf_Half attributeType) const;

    QList<DwarfDie*> children() const;
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

    mutable QList<DwarfDie*> m_children;
    mutable bool m_childrenScanned = false;
};

Q_DECLARE_METATYPE(DwarfDie*)

#endif // DWARFDIE_H
