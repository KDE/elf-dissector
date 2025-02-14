/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef FILEUSERMODEL_H
#define FILEUSERMODEL_H

#include <QAbstractListModel>

class ElfFileSet;
class ElfFile;

/** Lists all files using a given file from an ElfFileSet. */
class FileUserModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role {
        FileRole = Qt::UserRole + 1
    };
    explicit FileUserModel(QObject* parent = nullptr);
    ~FileUserModel();

    void setFile(ElfFileSet *fileSet, ElfFile *usedFile);
    ElfFile* usedFile() const;

    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    ElfFileSet *m_fileSet = nullptr;
    ElfFile *m_usedFile = nullptr;
    QList<int> m_users;
};

#endif // FILEUSERMODEL_H
