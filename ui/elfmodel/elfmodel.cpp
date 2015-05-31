/*
    Copyright (C) 2013-2014 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "elfmodel.h"

#include <elf/elffile.h>
#include <elf/elfheader.h>

#include "rowcountvisitor.h"
#include "indexvisitor.h"
#include "parentvisitor.h"
#include "datavisitor.h"

#include <QDebug>

ElfModel::ElfModel(QObject* parent) : QAbstractItemModel(parent)
{
}

ElfModel::~ElfModel()
{
    clearInternalPointerMap();
}

ElfFileSet* ElfModel::fileSet() const
{
    return m_fileSet;
}

void ElfModel::setFileSet(ElfFileSet *fileSet)
{
    beginResetModel();
    clearInternalPointerMap();
    m_fileSet = fileSet;

    auto *v = new ElfNodeVariant;
    v->payload = m_fileSet;
    v->type = ElfNodeVariant::FileSet;
    m_internalPointerMap.insert(m_fileSet, v);

    endResetModel();
}

void ElfModel::clearInternalPointerMap()
{
    for (auto it = m_internalPointerMap.cbegin(); it != m_internalPointerMap.cend(); ++it)
        delete it.value();
    m_internalPointerMap.clear();
}

QVariant ElfModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || !m_fileSet)
        return QVariant();

    ElfNodeVariant var = contentForIndex(index);

    DataVisitor v(m_fileSet);
    switch (index.column()) {
        case 0:
            return v.visit(&var, role);
        case 1:
            return v.visit(&var, role == Qt::DisplayRole ? SizeRole : role); // TODO proper formatting
    }

    return QVariant();
}

int ElfModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 2;
}

int ElfModel::rowCount(const QModelIndex& parent) const
{
    if (!m_fileSet || (parent.isValid() && parent.column() != 0))
        return 0;

    RowCountVisitor v;
    ElfNodeVariant var = contentForIndex(parent);
    return v.visit(&var);
}

QModelIndex ElfModel::parent(const QModelIndex& child) const
{
    if (!m_fileSet || !child.isValid())
        return QModelIndex();

    ParentVisitor v(this);
    auto parentData = v.visit(variantForIndex(child));
    if (!parentData.first)
        return QModelIndex();
    Q_ASSERT(m_internalPointerMap.contains(parentData.first));
    return createIndex(parentData.second, 0, m_internalPointerMap.value(parentData.first));
}

QModelIndex ElfModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!m_fileSet || !hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid())
        return createIndex(row, column, m_internalPointerMap.value(m_fileSet));

    IndexVisitor v;
    auto childValue = v.visit(variantForIndex(parent), parent.row());
    ElfNodeVariant *var = makeVariant(childValue.first, childValue.second);
    return createIndex(row, column, var);
}

QVariant ElfModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return tr("Entry");
            case 1: return tr("Size");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

ElfNodeVariant* ElfModel::variantForIndex(const QModelIndex& index) const
{
    return static_cast<ElfNodeVariant*>(index.internalPointer());
}

ElfNodeVariant ElfModel::contentForIndex(const QModelIndex& index) const
{
    if (!index.isValid())
        return *m_internalPointerMap.value(m_fileSet);

    ElfNodeVariant *parentVar = variantForIndex(index);

    IndexVisitor v;
    auto childData = v.visit(parentVar, index.row());
    ElfNodeVariant var;
    var.payload = childData.first;
    var.type = childData.second;
    return var;
}

ElfNodeVariant* ElfModel::makeVariant(void* payload, ElfNodeVariant::Type type) const
{
    auto it = m_internalPointerMap.constFind(payload);
    if (it != m_internalPointerMap.cend())
        return it.value();
    ElfNodeVariant *var = new ElfNodeVariant;
    var->payload = payload;
    var->type = type;
    m_internalPointerMap.insert(payload, var);
    return var;
}
