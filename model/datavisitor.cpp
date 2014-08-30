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
                s += "</tt>";
            }
            return s;
        }
    }

    return QVariant();
}

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
            if (entry->isStringValue())
                s+= entry->stringValue();
            else
                s += QString::number(entry->value());
            s += "<br/>";
            return s;
        }
    }

    return QVariant();
}