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
    QVector<int> m_users;
};

#endif // FILEUSERMODEL_H
