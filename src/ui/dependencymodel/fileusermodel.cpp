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

#include "fileusermodel.h"

#include <elf/elffileset.h>


FileUserModel::FileUserModel(QObject* parent): QAbstractListModel(parent)
{
}

FileUserModel::~FileUserModel() = default;

void FileUserModel::setFile(ElfFileSet* fileSet, ElfFile *usedFile)
{
    beginResetModel();
    const auto l = [](FileUserModel* m) { m->endResetModel(); };
    const auto endReset = std::unique_ptr<FileUserModel, decltype(l)>(this, l);

    m_fileSet = fileSet;
    m_usedFile = usedFile;
    m_users.clear();
    if (!m_fileSet || !m_usedFile)
        return;

    if (!usedFile->dynamicSection())
        return;
    auto soName = usedFile->dynamicSection()->soName();
    if (soName.isEmpty())
        soName = usedFile->fileName().toUtf8();

    for (int i = 0; i < fileSet->size(); ++i) {
        const auto file = fileSet->file(i);
        if (!file->dynamicSection())
            continue;
        foreach (const auto &dep, file->dynamicSection()->neededLibraries()) {
            if (dep == soName)
                m_users.push_back(i);
        }
    }
}

ElfFile* FileUserModel::usedFile() const
{
    return m_usedFile;
}

QVariant FileUserModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || !m_fileSet)
        return {};

    switch (role) {
        case Qt::DisplayRole:
            return m_fileSet->file(m_users.at(index.row()))->displayName();
        case FileRole:
            return QVariant::fromValue(m_fileSet->file(m_users.at(index.row())));
    }

    return {};
}

int FileUserModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid() || !m_fileSet)
        return 0;

    return m_users.size();
}

QVariant FileUserModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return tr("Users");
    return QAbstractItemModel::headerData(section, orientation, role);
}
