#include "elfmodel.h"

#include <elf/elffile.h>
#include <elf/elfheader.h>

#include "rowcountvisitor.h"
#include "indexvisitor.h"
#include "parentvisitor.h"

#include <QDebug>

ElfModel::ElfModel(QObject* parent) : QAbstractItemModel(parent)
{
}

ElfModel::~ElfModel()
{
    clearInternalPointerMap();
}

void ElfModel::setFile(const ElfFile::Ptr &file)
{
    beginResetModel();
    clearInternalPointerMap();
    m_file = file;

    auto *v = new ElfNodeVariant;
    v->payload = m_file.get();
    v->type = ElfNodeVariant::File;
    m_internalPointerMap.insert(m_file.get(), v);

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
    if (!index.isValid() || !m_file)
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
                return QString::fromLatin1(m_file->sectionHeaders().at(index.row())->name());
            case 1:
                return QString::number(m_file->sectionHeaders().at(index.row())->size()); // TODO proper formatting
        }
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
    if (!m_file || parent.isValid())
        return 0;
    RowCountVisitor v;
    ElfNodeVariant var;
    var.type = ElfNodeVariant::File;
    var.payload = m_file.get();
    return v.visit(&var);
}

QModelIndex ElfModel::parent(const QModelIndex& child) const
{
    if (!m_file)
        return QModelIndex();

    ParentVisitor v;
    if (!child.internalPointer())
        return QModelIndex();
    auto parentData = v.visit(static_cast<ElfNodeVariant*>(child.internalPointer()));
    if (!parentData.first)
        return QModelIndex();
    Q_ASSERT(m_internalPointerMap.contains(parentData.first));
    return createIndex(parentData.second, 0, m_internalPointerMap.value(parentData.first));
}

QModelIndex ElfModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!m_file)
        return QModelIndex();

    IndexVisitor v;
    auto childValue = v.visit(parent.isValid() ? static_cast<ElfNodeVariant*>(parent.internalPointer()) : m_internalPointerMap.value(m_file.get()), row);
    auto it = m_internalPointerMap.constFind(childValue.first);
    if (it != m_internalPointerMap.cend())
        return createIndex(row, column, it.value());
    ElfNodeVariant *var = new ElfNodeVariant;
    var->payload = childValue.first;
    var->type = childValue.second;
    m_internalPointerMap.insert(childValue.first, var);
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

#include "elfmodel.moc"
