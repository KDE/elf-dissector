/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <QAbstractListModel>

class ElfFileSet;

/** Simple flat list of all files in an ElfFileSet. */
class FileListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Role {
        FileIndexRole = Qt::UserRole + 1,
        FileRole
    };
    explicit FileListModel(QObject* parent = nullptr);
    ~FileListModel();

    ElfFileSet *fileSet() const;
    void setFileSet(ElfFileSet *fileSet);

    QVariant data(const QModelIndex& index, int role) const override;
    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    void computeUsageCounts(int idx);

    ElfFileSet *m_fileSet = nullptr;
    struct UseCounts {
        int fileCount = 0;
        int symCount = 0;
    };
    std::vector<UseCounts> m_useCounts;
};

#endif // FILELISTMODEL_H
