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

#include "filelistmodel.h"

#include <elf/elffileset.h>

FileListModel::FileListModel(QObject* parent): QAbstractListModel(parent)
{
}

FileListModel::~FileListModel() = default;

ElfFileSet* FileListModel::fileSet() const
{
    return m_fileSet;
}

void FileListModel::setFileSet(ElfFileSet* fileSet)
{
    beginResetModel();
    m_fileSet = fileSet;
    endResetModel();
}

QVariant FileListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || !m_fileSet)
        return {};

    switch (role) {
        case Qt::DisplayRole:
            return m_fileSet->file(index.row())->displayName();
        case FileIndexRole:
            return index.row();
        case FileRole:
            return QVariant::fromValue(m_fileSet->file(index.row()));
    }

    return {};
}

int FileListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid() || !m_fileSet)
        return 0;
    return m_fileSet->size();
}

QVariant FileListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return tr("Shared Object");
    return QAbstractItemModel::headerData(section, orientation, role);
}
