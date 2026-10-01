/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "filelistmodel.h"

#include <elf/elffileset.h>

#include <checks/dependenciescheck.h>

#include <QTimer>

FileListModel::FileListModel(QObject* parent): QAbstractTableModel(parent)
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
    m_useCounts.clear();
    m_useCounts.resize(fileSet->size());
    endResetModel();
    QTimer::singleShot(0, this, [this]() { computeUsageCounts(0); });
}

void FileListModel::computeUsageCounts(int idx)
{
    // FIXME this would crash if m_fileSet is destroyed while we are still recomputing
    if (!m_fileSet || idx >= m_fileSet->size()) {
        return;
    }

    // this assumes a topologically sorted input
    for (auto j = idx + 1; j < m_fileSet->size(); ++j) {
        const auto l = DependenciesCheck::usedSymbols(m_fileSet->file(idx), m_fileSet->file(j));
        if (!l.isEmpty()) {
            m_useCounts[j].fileCount++;
            m_useCounts[j].symCount += l.size();
        }
    }

    Q_EMIT dataChanged(index(0, 1), index(rowCount() - 1, 2));
    QTimer::singleShot(0, this, [this, idx]() { computeUsageCounts(idx + 1); });
}

QVariant FileListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || !m_fileSet)
        return {};

    switch (role) {
        case Qt::DisplayRole:
            switch (index.column()) {
                case 0:
                    return m_fileSet->file(index.row())->displayName();
                case 1:
                    return m_useCounts[index.row()].fileCount;
                case 2:
                    return m_useCounts[index.row()].symCount;
            }
            break;
        case FileIndexRole:
            return index.row();
        case FileRole:
            return QVariant::fromValue(m_fileSet->file(index.row()));
    }

    return {};
}

int FileListModel::columnCount([[maybe_unused]] const QModelIndex &parent) const
{
    return 3;
}

int FileListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid() || !m_fileSet)
        return 0;
    return m_fileSet->size();
}

QVariant FileListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return tr("Shared Object");
            case 1: return tr("Files");
            case 2: return tr("Symbols");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}
