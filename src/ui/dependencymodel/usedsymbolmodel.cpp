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

#include "usedsymbolmodel.h"

#include <elf/elffile.h>
#include <elf/elfsymboltableentry.h>
#include <elf/elfsymboltablesection.h>
#include <elf/elfhashsection.h>

#include <demangle/demangler.h>
#include <checks/dependenciescheck.h>

#include <cassert>

UsedSymbolModel::UsedSymbolModel(QObject* parent): QAbstractListModel(parent)
{
}

UsedSymbolModel::~UsedSymbolModel() = default;

void UsedSymbolModel::setFiles(ElfFile* user, ElfFile* provider)
{
    beginResetModel();
    const auto l = [](UsedSymbolModel* m) { m->endResetModel(); };
    const auto endReset = std::unique_ptr<UsedSymbolModel, decltype(l)>(this, l);

    m_entries.clear();
    if (!user || !provider)
        return;

    m_entries = DependenciesCheck::usedSymbols(user, provider);
}

QVariant UsedSymbolModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return {};

    switch (role) {
        case Qt::DisplayRole:
            return Demangler::demangleFull(m_entries.at(index.row())->name());
            break;
    }

    return {};
}

int UsedSymbolModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return m_entries.size();
}

QVariant UsedSymbolModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("Symbol");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}
