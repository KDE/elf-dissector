/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "loadbenchmarkmodel.h"

#include <checks/ldbenchmark.h>
#include <elf/elffile.h>

LoadBenchmarkModel::LoadBenchmarkModel(QObject* parent): QAbstractTableModel(parent)
{
}

LoadBenchmarkModel::~LoadBenchmarkModel() = default;

void LoadBenchmarkModel::setBenchmark(const std::shared_ptr< LDBenchmark >& data)
{
    beginResetModel();
    m_data = data;
    endResetModel();
}

QVariant LoadBenchmarkModel::data(const QModelIndex& index, int role) const
{
    if (!m_data || !index.isValid())
        return {};

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
            case 0: return m_data->file(index.row())->displayName();
            case 1: return m_data->median(LDBenchmark::LoadMode::Lazy, index.row());
            case 2: return m_data->min(LDBenchmark::LoadMode::Lazy, index.row());
            case 3: return m_data->median(LDBenchmark::LoadMode::Now, index.row());
            case 4: return m_data->min(LDBenchmark::LoadMode::Now, index.row());
            case 5: return m_data->file(index.row())->reverseRelocator()->size();
        }
    }
    return {};
}

int LoadBenchmarkModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 6;
}

int LoadBenchmarkModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid() || !m_data)
        return 0;
    return m_data->size();
}

QVariant LoadBenchmarkModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return tr("File");
            case 1: return tr("Lazy Median");
            case 2: return tr("Lazy Min");
            case 3: return tr("Now Median");
            case 4: return tr("Now Min");
            case 5: return tr("Relocs");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}
