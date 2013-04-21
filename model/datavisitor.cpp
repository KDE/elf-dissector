#include "datavisitor.h"
#include "elfmodel.h"

#include <elf/elffile.h>
#include <elf.h>

#include <QObject>
#include <QStringBuilder>

QVariant DataVisitor::doVisit(ElfFile* file, int arg) const
{
    switch (arg) {
        case Qt::DisplayRole:
            return file->displayName();
        case ElfModel::SizeRole:
            return QVariant::fromValue<uint64_t>(file->size());
        case ElfModel::DetailRole:
        {
            QString s;
            s += "File name: " + file->displayName() + "<br/>";
            s += QStringLiteral("Address size: ") + (file->type() == ELFCLASS32 ? "32 bit" : "64 bit") + "<br/>";
            return s;
        }
    }
    return QVariant();
}

QVariant DataVisitor::doVisit(ElfSection* section, int arg) const
{
    switch (arg) {
        case Qt::DisplayRole:
            if (*section->header()->name() == 0)
                return QObject::tr("<null>");
            return section->header()->name();
        case ElfModel::SizeRole:
            return QVariant::fromValue<uint64_t>(section->size());
    }

    return QVariant();
}

static QString bindTypeToString(uint8_t bindType)
{
    switch (bindType) {
        case STB_LOCAL: return "local";
        case STB_GLOBAL: return "global";
        case STB_WEAK: return "weak";
        case STB_NUM: return "number of defined types"; // ???
        case STB_GNU_UNIQUE: return "GNU unique";
        default: QObject::tr("unknown (%1)").arg(bindType);
    }
    return QString();
}

static QString symbolTypeToString(uint8_t symbolType)
{
    switch (symbolType) {
        case STT_NOTYPE: return "unspecified";
        case STT_OBJECT: return "data object";
        case STT_FUNC: return "code object";
        case STT_SECTION: return "section";
        case STT_FILE: return "file name";
        case STT_COMMON: return "common data object";
        case STT_TLS: return "thread-local data object";
        case STT_NUM: return "number of defined types";
        case STT_GNU_IFUNC: return "GNU indirect code object";
        default: QObject::tr("unknown (%1)").arg(symbolType);
    }
    return QString();
}

QVariant DataVisitor::doVisit(ElfSymbolTableEntry* entry, int arg) const
{
    switch (arg) {
        case Qt::DisplayRole:
            if (*entry->name() == 0)
                return QObject::tr("<null>");
            return entry->name();
        case ElfModel::SizeRole:
            return QVariant::fromValue<uint64_t>(entry->size());
        case ElfModel::DetailRole:
        {
            QString s;
            s += QStringLiteral("Mangled name: ") + entry->name() + "<br/>";
            s += QStringLiteral("Size: ") + QString::number(entry->size()) + "<br/>";
            s += QStringLiteral("Value: 0x") + QString::number(entry->value(), 16) + "<br/>";
            s += QStringLiteral("Bind type: ") + bindTypeToString(entry->bindType()) + "<br/>";
            s += QStringLiteral("Symbol type: ") + symbolTypeToString(entry->type()) + "<br/>";
            return s;
        }
    }

    return QVariant();
}
