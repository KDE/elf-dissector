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

#ifndef ELFMODEL_H
#define ELFMODEL_H

#include "elfnodevariant.h"

#include <QAbstractItemModel>

class ElfFileSet;
class ElfSection;
class ElfSymbolTableEntry;
class ElfGotEntry;
class ElfPltEntry;
class DwarfDie;

/** Model for the ELF structure.
 *
 * Internal pointer is the ElfNodeVariant pointer containing the _parent_ for the node.
 */
class ElfModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum Role {
        SizeRole = Qt::UserRole + 1,
        DetailRole,
        FileRole,
        SectionRole,
        NodeUrl
    };

    explicit ElfModel(QObject* parent = nullptr);
    ~ElfModel();

    ElfFileSet* fileSet() const;
    void setFileSet(ElfFileSet* fileSet);

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QModelIndex indexForNode(ElfSection* section) const;
    QModelIndex indexForNode(ElfSymbolTableEntry* symbol) const;
    QModelIndex indexForNode(ElfGotEntry *entry) const;
    QModelIndex indexForNode(ElfPltEntry *entry) const;
    QModelIndex indexForNode(DwarfDie* die) const;
    QModelIndex indexForUrl(const QUrl &url) const;

private:
    friend class ParentVisitor;

    QModelIndex indexForNode(void* payload, ElfNodeVariant::Type type) const;

    void clearInternalPointerMap();
    ElfNodeVariant* variantForIndex(const QModelIndex &index) const;
    ElfNodeVariant contentForIndex(const QModelIndex& index) const;
    ElfNodeVariant* makeVariant(void* payload, ElfNodeVariant::Type type) const;

    QUrl urlForIndex(const QModelIndex &index) const;

private:
    ElfFileSet *m_fileSet = nullptr;
    mutable QHash<void*, ElfNodeVariant*> m_internalPointerMap;

};

#endif // ELFMODEL_H
