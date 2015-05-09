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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "typemodel.h"

#include <navigator/codenavigatorprinter.h>
#include <elf/elffileset.h>
#include <dwarf/dwarfinfo.h>
#include <dwarf/dwarfdie.h>
#include <printers/dwarfprinter.h>

#include <libdwarf/dwarf.h>

#include <QTime>

TypeModel::TypeModel(QObject* parent): QAbstractItemModel(parent)
{
    setFileSet(nullptr);
}

TypeModel::~TypeModel() = default;

void TypeModel::setFileSet(ElfFileSet* fileSet)
{
    QTime t;
    t.start();
    beginResetModel();
    const auto l = [](TypeModel* m) { m->endResetModel(); };
    const auto endReset = std::unique_ptr<TypeModel, decltype(l)>(this, l);

    m_childMap.clear();
    m_parentMap.clear();
    m_nodes.clear();
    m_nodes.resize(1);
    m_childMap.resize(1);

    if (!fileSet)
        return;

    for (int i = 0; i < fileSet->size(); ++i)
        addFile(fileSet->file(i));

    qDebug() << "Found" << m_nodes.size() << "types, took" << t.elapsed() << "ms.";
}

void TypeModel::addFile(ElfFile* file)
{
    const auto dwarf = file->dwarfInfo();
    if (!dwarf)
        return;

    for (const auto cu : dwarf->compilationUnits()) {
        for (const auto die : cu->children())
            addTopLevelDwarfDie(die);
    }
}

void TypeModel::addTopLevelDwarfDie(DwarfDie* die)
{
    if (die->tag() != DW_TAG_structure_type && die->tag() != DW_TAG_class_type)
        return; // TODO support namespaces and nested types

    auto& children = m_childMap[0];
    const auto dieName = die->name();
    const auto it = std::lower_bound(children.begin(), children.end(), dieName, [this](uint32_t nodeId, const QByteArray &dieName) {
        return m_nodes.at(nodeId).die->name() < dieName;
    });
    if (it != children.constEnd() && m_nodes.at((*it)).die->name() == dieName)
        return; // TODO pick the one with more information here!

    const uint32_t nodeId = m_nodes.size();
    m_nodes.resize(nodeId + 1);
    m_nodes[nodeId].die = die;
    children.insert(it, nodeId);
    m_parentMap.resize(nodeId + 1);
    m_parentMap[nodeId] = 0;
}

int TypeModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 1;
}

int TypeModel::rowCount(const QModelIndex& parent) const
{
    // TODO
    if (!parent.isValid())
        return m_childMap.at(0).size();
    return 0;
}

QVariant TypeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return {};

    const auto node = m_nodes.at(index.internalId());
    switch (role) {
        case Qt::DisplayRole:
            return node.die->name();
        case TypeModel::DetailRole:
        {
            QString s = DwarfPrinter::dieRichText(node.die);
            s += CodeNavigatorPrinter::sourceLocationRichText(node.die);
            return s;
        }
    };

    return {};
}

QModelIndex TypeModel::index(int row, int column, const QModelIndex& parent) const
{
    const int32_t parentId = parent.internalId();
    if (row < 0 || column < 0 || m_childMap.size() <= parentId || m_childMap.at(parentId).size() <= row)
        return {};

    return createIndex(row, column, m_childMap.at(parentId).at(row));
}

QModelIndex TypeModel::parent(const QModelIndex& child) const
{
    return {};
}

QVariant TypeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return tr("Data Type");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}
