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

#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <QAbstractListModel>

class ElfFileSet;

/** Simple flat list of all files in an ElfFileSet. */
class FileListModel : public QAbstractListModel
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
    int rowCount(const QModelIndex& parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    ElfFileSet *m_fileSet = nullptr;
};

#endif // FILELISTMODEL_H
