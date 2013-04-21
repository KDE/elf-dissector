#include "datavisitor.h"
#include "elfmodel.h"

#include <elf/elffile.h>

QVariant DataVisitor::doVisit(ElfFile* file, int arg) const
{
    switch (arg) {
        case Qt::DisplayRole:
            return file->displayName();
        case ElfModel::SizeRole:
            return QVariant::fromValue<uint64_t>(file->size());
    }
    return QVariant();
}

QVariant DataVisitor::doVisit(ElfSection* section, int arg) const
{
    switch (arg) {
        case Qt::DisplayRole:
            return section->header()->name();
        case ElfModel::SizeRole:
            return QVariant::fromValue<uint64_t>(section->size());
    }

    return QVariant();
}

QVariant DataVisitor::doVisit(ElfSymbolTableEntry* entry, int arg) const
{
    switch (arg) {
        case Qt::DisplayRole:
            return entry->name();
        case ElfModel::SizeRole:
            return QVariant::fromValue<uint64_t>(entry->size());
    }

    return QVariant();
}
