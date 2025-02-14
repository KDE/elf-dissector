/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TYPEMODEL_H
#define TYPEMODEL_H

#include <QAbstractItemModel>
#include <QList>

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
    QList<QList<uint32_t>> m_childMap;
    QList<uint32_t> m_parentMap;
    QList<Node> m_nodes;

    ElfFileSet *m_fileSet = nullptr;
    bool m_hasInvalidDies;
};

#endif // TYPEMODEL_H
