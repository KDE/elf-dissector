/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
