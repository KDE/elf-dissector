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

#ifndef TYPEMODEL_H
#define TYPEMODEL_H

#include <QAbstractItemModel>
#include <QVector>

class ElfFileSet;
class ElfFile;
class DwarfDie;

/** All data types found in a ELF file set. */
class TypeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum Role {
        DetailRole = Qt::UserRole + 1
    };

    explicit TypeModel(QObject* parent = nullptr);
    ~TypeModel();

    void setFileSet(ElfFileSet *fileSet);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool hasInvalidDies() const { return m_hasInvalidDies; }
private:
    void addFile(ElfFile *file);
    bool addDwarfDieRecursive(DwarfDie* die, uint32_t parentId);

    // the tree hierarchy is built using 32bit sequential ids, which act as index for the node struct
    struct Node {
        DwarfDie *die = nullptr;
    };
    QVector<QVector<uint32_t>> m_childMap;
    QVector<uint32_t> m_parentMap;
    QVector<Node> m_nodes;

    ElfFileSet *m_fileSet = nullptr;
    bool m_hasInvalidDies;
};

#endif // TYPEMODEL_H
