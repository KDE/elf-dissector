#include "elfmodel.h"

#include <elf/elffile.h>
#include <elf/elfheader.h>

#include <QDebug>
#include <boost/graph/graph_concepts.hpp>

ElfModel::ElfModel(QObject* parent) : QAbstractItemModel(parent)
{
}

ElfModel::~ElfModel()
{
}

void ElfModel::setFile(const ElfFile::Ptr &file)
{
    beginResetModel();
    m_file = file;
    endResetModel();
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
    return 2;
}

int ElfModel::rowCount(const QModelIndex& parent) const
{
    if (!m_file || parent.isValid())
        return 0;
    return m_file->header()->sectionHeaderCount();
}

QModelIndex ElfModel::parent(const QModelIndex& child) const
{
    return QModelIndex();
}

QModelIndex ElfModel::index(int row, int column, const QModelIndex& parent) const
{
    return createIndex(row, column);
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
