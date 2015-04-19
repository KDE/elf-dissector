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

#include "datavisitor.h"
#include "elfmodel.h"

#include <elf/elffile.h>
#include <elf.h>

#include <disassmbler/disassembler.h>
#include <demangle/demangler.h>
#include <checks/structurepackingcheck.h>

#include <QDebug>
#include <QObject>
#include <QStringBuilder>

#include <libdwarf/dwarf.h>

static QString machineToString(uint16_t machineType)
{
#define M(x) case EM_ ## x: return QStringLiteral("" #x);
    switch (machineType) {
        M(NONE)
        M(386)
        M(ARM)
        M(X86_64)
        M(AVR)
        M(AARCH64)
    }
    return QStringLiteral("Unknown machine type: " ) + QString::number(machineType);
#undef M
}

DataVisitor::DataVisitor(ElfFileSet* fileSet) : m_fileSet(fileSet)
{
}

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
            s += QStringLiteral("Byte order: ") + (file->byteOrder() == ELFDATA2LSB ? "little endian" : "big endian") + "<br/>";
            s += QStringLiteral("Machine: ") + machineToString(file->header()->machine()) + "<br/>";
            return s;
        }
    }
    return QVariant();
}

static QString sectionTypeToString(uint32_t sectionType)
{
    switch (sectionType) {
        case SHT_NULL: return "&lt;null&gt;";
        case SHT_PROGBITS: return "program data";
        case SHT_SYMTAB: return "symbol table";
        case SHT_STRTAB: return "string table";
        case SHT_RELA: return "relocation entries with addends";
        case SHT_HASH: return "symbol hash table";
        case SHT_DYNAMIC: return "dynamic linking information";
        case SHT_NOTE: return "notes";
        case SHT_NOBITS: return "bss";
        case SHT_REL: return "relocation entries, no addends";
        case SHT_SHLIB: return "reserved";
        case SHT_DYNSYM: return "dynamic linker symbol table";
        case SHT_INIT_ARRAY: return "array of constructors";
        case SHT_FINI_ARRAY: return "array of destructors";
        case SHT_PREINIT_ARRAY: return "array of preconstructors";
        case SHT_GROUP: return "section group";
        case SHT_SYMTAB_SHNDX: return "extended section indices";

        case SHT_GNU_ATTRIBUTES: return "GNU object attributes";
        case SHT_GNU_HASH: return "GNU-style hash table";
        case SHT_GNU_LIBLIST: return "GNU prelink library list";
        case SHT_CHECKSUM: return "checksum for DSO conent";
        case SHT_GNU_verdef: return "GNU version definition";
        case SHT_GNU_verneed: return "GNU version needs";
        case SHT_GNU_versym: return "GNU version symbol table";

        default: QObject::tr("unknown (0x%1)").arg(sectionType, 16);
    }

    return QString();
}

static QString sectionFlagsToString(uint64_t flags)
{
    QStringList s;
    if (flags & SHF_WRITE) s.push_back(QObject::tr("writable"));
    if (flags & SHF_ALLOC) s.push_back(QObject::tr("occupies memory during execution"));
    if (flags & SHF_EXECINSTR) s.push_back(QObject::tr("executable"));
    if (flags & SHF_MERGE) s.push_back(QObject::tr("might be merged"));
    if (flags & SHF_STRINGS) s.push_back(QObject::tr("contains nul-terminated strings"));
    if (flags & SHF_INFO_LINK) s.push_back(QObject::tr("sh_info contains SHT index"));
    if (flags & SHF_LINK_ORDER) s.push_back(QObject::tr("preserve order after combining"));
    if (flags & SHF_OS_NONCONFORMING) s.push_back(QObject::tr("non-standard OS specific handling required"));
    if (flags & SHF_GROUP) s.push_back(QObject::tr("group member"));
    if (flags & SHF_TLS) s.push_back(QObject::tr("holds thread-local data"));
    if (s.isEmpty())
        return QObject::tr("&lt;none&gt;");
    return s.join(", ");
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
        case ElfModel::DetailRole:
        {
            QString s;
            s += QStringLiteral("Name: ") + section->header()->name() + "<br/>";
            s += QStringLiteral("Size: ") + QString::number(section->header()->size()) + " bytes<br/>";
            s += QStringLiteral("Flags: ") + sectionFlagsToString(section->header()->flags()) + "<br/>";
            s += QStringLiteral("Offset: 0x") + QString::number(section->header()->sectionOffset(), 16) + "<br/>";
            s += QStringLiteral("Virtual Address: 0x") + QString::number(section->header()->virtualAddress(), 16) + "<br/>";
            s += QStringLiteral("Type: ") + sectionTypeToString(section->header()->type()) + "<br/>";
            if (section->header()->link())
                s += QStringLiteral("Linked section: ") + section->linkedSection<ElfSection>()->header()->name() + "<br/>";
            if (section->header()->entrySize()) {
                s += QStringLiteral("Entries: ") + QString::number(section->header()->entryCount())
                  + " x " + QString::number(section->header()->entrySize()) + " byte<br/>";
            }
            return s;
        }
    }

    return QVariant();
}

QVariant DataVisitor::doVisit(ElfSymbolTableSection* symtab, int arg) const
{
    const auto base = doVisit(static_cast<ElfSection*>(symtab), arg);
    if (arg != ElfModel::DetailRole)
        return base;
    QString s = base.toString();
    s += "<br/>Exported symbols: " + QString::number(symtab->exportCount());
    s += "<br/>Imported symbols: " + QString::number(symtab->importCount());
    s += "<br/>";
    return s;
}


static QString bindTypeToString(uint8_t bindType)
{
    switch (bindType) {
        case STB_LOCAL: return "local";
        case STB_GLOBAL: return "global";
        case STB_WEAK: return "weak";
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
        case STT_GNU_IFUNC: return "GNU indirect code object";
        default: QObject::tr("unknown (%1)").arg(symbolType);
    }
    return QString();
}

static QString visibilityToString(uint8_t visibility)
{
    switch (visibility) {
        case STV_DEFAULT: return "default";
        case STV_INTERNAL: return "internal";
        case STV_HIDDEN: return "hidden";
        case STV_PROTECTED: return "protected";
        default: QObject::tr("unknown (%1)").arg(visibility);
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
            Demangler demangler;
            s += QStringLiteral("Demangled name: ") + demangler.demangleFull(entry->name()) + "<br/>";
            s += QStringLiteral("Size: ") + QString::number(entry->size()) + "<br/>";
            s += QStringLiteral("Value: 0x") + QString::number(entry->value(), 16) + "<br/>";
            s += QStringLiteral("Bind type: ") + bindTypeToString(entry->bindType()) + "<br/>";
            s += QStringLiteral("Symbol type: ") + symbolTypeToString(entry->type()) + "<br/>";
            s += QStringLiteral("Visibility: ") + visibilityToString(entry->visibility()) + "<br/>";
            if (entry->sectionIndex() < entry->symbolTable()->file()->header()->sectionHeaderCount())
                s += QStringLiteral("Section: ") + entry->symbolTable()->file()->sectionHeaders().at(entry->sectionIndex())->name() + "<br/>";
            // TODO: they way we interpret value() is wrong, seems to refer to program header vm addresses
            if (entry->type() == STT_FUNC && entry->size() > 0) {
                Disassembler da;
                s += QStringLiteral("Code:<br/><tt>") + da.disassemble(entry) + "</tt>";
            } else if (entry->type() == STT_OBJECT && entry->size() > 0) {
                s += QStringLiteral("Data:<br/><tt>");
                const unsigned char* data = entry->data();
                s += QByteArray::fromRawData((const char*)data, entry->size()).toHex();
                s += "</tt><br/>";
            }
            auto *dwarf = entry->symbolTable()->file()->dwarfInfo();
            if (dwarf) {
                s += QStringLiteral("Source location: ") + dwarf->sourceLocationForMangledSymbol(entry->name()) + "<br/>";
            }
            return s;
        }
    }

    return QVariant();
}

#define DYN_FLAG(flag, name) \
    if (flags & flag) l.push_back(name); \
    handledFlags |= flag

static QString dtFlagsToString(uint64_t flags)
{
    QStringList l;
    uint64_t handledFlags = 0;

    DYN_FLAG(DF_ORIGIN, "object may use DF_ORIGIN");
    DYN_FLAG(DF_SYMBOLIC, "symbol resolutions starts here");
    DYN_FLAG(DF_TEXTREL, "objects contains text relocations");
    DYN_FLAG(DF_BIND_NOW, "bind now");
    DYN_FLAG(DF_STATIC_TLS, "module uses the static TLS model");

    if (flags & ~handledFlags)
        l.push_back(QString("unhandled flags 0x%1").arg(flags & ~handledFlags, 16));

    if (l.isEmpty())
        return "<none>";
    return l.join(", ");
}

static QString dtFlags1ToString(uint64_t flags)
{
    QStringList l;
    uint64_t handledFlags = 0;

    DYN_FLAG(DF_1_NOW, "set RTLD_NOW for this object");
    DYN_FLAG(DF_1_GLOBAL, "set RTLD_GLOBAL for this object");
    DYN_FLAG(DF_1_GROUP, "set RTLD_GROUP for this object");
    DYN_FLAG(DF_1_NODELETE, "set RTLD_NODELETE for this object");
    DYN_FLAG(DF_1_LOADFLTR, "trigger filtee loading at runtime");
    DYN_FLAG(DF_1_INITFIRST, "set RTLD_INITFIRST for this object");
    DYN_FLAG(DF_1_NOOPEN, "set RTLD_NOOPEN for this object");

    if (flags & ~handledFlags)
        l.push_back(QString("unhandled flags 0x%1").arg(flags & ~handledFlags, 16));

    if (l.isEmpty())
        return "<none>";
    return l.join(", ");
}

#undef DYN_FLAG

QVariant DataVisitor::doVisit(ElfDynamicEntry *entry, int arg) const
{
    switch (arg) {
        case Qt::DisplayRole:
            return entry->tagName();
        case ElfModel::DetailRole:
        {
            QString s;
            s += QStringLiteral("Tag name: ") + entry->tagName() + "<br/>";
            s += QStringLiteral("Value: ");
            switch (entry->tag()) {
                case DT_FLAGS:
                    s += dtFlagsToString(entry->value());
                    break;
                case DT_FLAGS_1:
                    s += dtFlags1ToString(entry->value());
                    break;
                default:
                    if (entry->isStringValue())
                        s+= entry->stringValue();
                    else
                        s += QString::number(entry->value());
            }
            s += "<br/>";
            return s;
        }
    }

    return QVariant();
}

QVariant DataVisitor::doVisit(DwarfInfo* info, int arg) const
{
    const auto sectionIndex = info->elfFile()->indexOfSection(".debug_info");
    const auto section = info->elfFile()->section<ElfSection>(sectionIndex);
    return doVisit(section, arg);
}

QVariant DataVisitor::doVisit(DwarfDie* die, int arg) const
{
    switch (arg) {
        case Qt::DisplayRole:
            return die->displayName();
        case ElfModel::DetailRole:
        {
            QString s;
            s += "TAG: " + QString::fromLocal8Bit(die->tagName()) + "<br/>";
            s += "Offset: " + QString::number(die->offset()) + "<br/>";
            foreach (const auto &attrType, die->attributes()) {
                const QVariant attrValue = die->attribute(attrType);
                QString attrValueStr;
                if (DwarfDie *die = attrValue.value<DwarfDie*>())
                    attrValueStr = die->displayName();
                else
                    attrValueStr = attrValue.toString();
                s += QString::fromLocal8Bit(die->attributeName(attrType)) + ": " + attrValueStr.toHtmlEscaped() + "<br/>";
            }

            if ((die->tag() == DW_TAG_structure_type || die->tag() == DW_TAG_class_type) && die->typeSize() > 0) {
                s += "<tt><pre>";
                StructurePackingCheck check;
                check.setElfFileSet(m_fileSet);
                s += check.checkOneStructure(die).toHtmlEscaped();
                s += "</pre></tt><br/>";
            }

            return s;
        }
    }
    return {};
}
