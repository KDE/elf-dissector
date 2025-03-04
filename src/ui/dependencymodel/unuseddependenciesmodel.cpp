/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "unuseddependenciesmodel.h"
#include <elf/elffileset.h>

#include <memory>

UnusedDependenciesModel::UnusedDependenciesModel(QObject* parent): QAbstractTableModel(parent)
{
}

UnusedDependenciesModel::~UnusedDependenciesModel() = default;

void UnusedDependenciesModel::setFileSet(ElfFileSet* fileSet)
{
    beginResetModel();
    const auto l = [](decltype(this) m) { m->endResetModel(); };
    const auto endReset = std::unique_ptr<UnusedDependenciesModel, decltype(l)>(this, l);

    m_fileSet = fileSet;
    m_unusedDeps.clear();
}

void UnusedDependenciesModel::findUnusedDependencies()
{
    if (!m_fileSet || !m_unusedDeps.isEmpty())
        return;

    beginResetModel();
    const auto l = [](decltype(this) m) { m->endResetModel(); };
    const auto endReset = std::unique_ptr<UnusedDependenciesModel, decltype(l)>(this, l);

    m_unusedDeps = DependenciesCheck::unusedDependencies(m_fileSet);
}

QVariant UnusedDependenciesModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || !m_fileSet || m_unusedDeps.isEmpty())
        return {};

    const auto unusedDep = m_unusedDeps.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            switch (index.column()) {
                case 0:
                    return m_fileSet->file(unusedDep.first)->displayName();
                case 1:
                    return m_fileSet->file(unusedDep.second)->displayName();
            }
            break;
    }

    return {};
}

int UnusedDependenciesModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 2;
}

int UnusedDependenciesModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return m_unusedDeps.size();
}

QVariant UnusedDependenciesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return tr("ELF File");
            case 1: return tr("Unused Dependency");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}
