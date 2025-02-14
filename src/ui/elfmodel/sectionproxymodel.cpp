/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "sectionproxymodel.h"
#include "elfmodel.h"

SectionProxyModel::SectionProxyModel(QObject* parent): QSortFilterProxyModel(parent)
{
}

SectionProxyModel::~SectionProxyModel() = default;

bool SectionProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    const auto sourceIndex = sourceModel()->index(source_row, 0, source_parent);
    const auto fileData = sourceModel()->data(sourceIndex, ElfModel::FileRole);
    const auto secData = sourceModel()->data(sourceIndex, ElfModel::SectionRole);

    if (fileData.isNull() && secData.isNull()) {
        return false;
    } else if (QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent)) {
        return true;
    }
    // implement our own recursive filtering,
    // code taken from KRecursiveFilterProxyModel::filterAcceptsRow()
    bool accepted = false;

    const int numChildren = sourceModel()->rowCount(sourceIndex);
    for (int row = 0, rows = numChildren; row < rows; ++row) {
        if (filterAcceptsRow(row, sourceIndex)) {
            accepted = true;
            break;
        }
    }
    return accepted;
}
