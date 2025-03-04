/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DEPENDENCYMODEL_H
#define DEPENDENCYMODEL_H

#include <QAbstractItemModel>
#include <QHash>
#include <QList>

class ElfFileSet;
class ElfFile;

/** Model showing full hierarchical dependencies of a file set. */
class DependencyModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit DependencyModel(QObject* parent = nullptr);
    ~DependencyModel();

    enum Role {
        UserFileRole = Qt::UserRole + 1,
        ProviderFileRole
    };

    ElfFileSet* fileSet() const;
    void setFileSet(ElfFileSet *fileSet);

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const final override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const final override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const final override;
    QModelIndex parent(const QModelIndex& child) const final override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const final override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const final override;

private:
    // we use an sequential int for the unique node index, the second have of the QMI internalId is the index of the file
    uint64_t makeId(uint32_t id, int32_t fileIndex) const;
    int32_t fileIndex(uint64_t qmiId) const;
    int32_t fileIndex(const QByteArray &needed) const;
    uint32_t nodeId(uint64_t qmiId) const;
    bool hasCycle(const QModelIndex &index) const;

    ElfFileSet *m_fileSet = nullptr;
    QHash<QByteArray, int32_t> m_fileIndex;
    mutable QList<uint64_t> m_parentMap;
    mutable QList<QList<uint64_t>> m_childMap;
    mutable uint32_t m_uniqueIndex = 0; // 0 is the invisible root
    static const int32_t InvalidFile = -1; // marker for dependencies we could not find

    int usedSymbolCount(int parentId, int fileId) const;
    mutable QList<QList<int>> m_symbolCountTable;
};

#endif // DEPENDENCYMODEL_H
