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
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "elfdynamicentry.h"
#include "elfdynamicsection.h"
#include "elfstringtablesection.h"
#include <QObject>

#include <elf.h>

#include <cassert>

ElfDynamicEntry::ElfDynamicEntry(const ElfDynamicSection* section) : m_section(section)
{
}

ElfDynamicEntry::~ElfDynamicEntry()
{
}

const ElfDynamicSection* ElfDynamicEntry::dynamicSection() const
{
    return m_section;
}

QString ElfDynamicEntry::tagName() const
{
    switch (tag()) {
        case DT_NULL: return QStringLiteral("<null>");
        case DT_NEEDED: return QStringLiteral("Needed library");
        case DT_PLTRELSZ: return QStringLiteral("PLT reloc size");
        case DT_PLTGOT: return QStringLiteral("PLTGOT");
        case DT_HASH: return QStringLiteral("Symbol hash table address");
        case DT_STRTAB: return QStringLiteral("String table address");
        case DT_SYMTAB: return QStringLiteral("Symbol table address");
        case DT_RELA: return QStringLiteral("Rela reloc address");
        case DT_RELASZ: return QStringLiteral("Rela reloc size");
        case DT_RELAENT: return QStringLiteral("Rela reloc entry size");
        case DT_STRSZ: return QStringLiteral("String table size");
        case DT_SYMENT: return QStringLiteral("Symbol table entry size");
        case DT_INIT: return QStringLiteral("Init function address");
        case DT_FINI: return QStringLiteral("Termination function address");
        case DT_SONAME: return QStringLiteral("Shared object name");
        case DT_RPATH: return QStringLiteral("RPATH");
        case DT_SYMBOLIC: return QStringLiteral("Symbol search start");
        case DT_REL: return QStringLiteral("Rel reloc address");
        case DT_RELSZ: return QStringLiteral("Rel reloc size");
        case DT_RELENT: return QStringLiteral("Rel reloc entry size");
        case DT_PLTREL: return QStringLiteral("Reloc type in PLT");
        case DT_DEBUG: return QStringLiteral("Debug");
        case DT_TEXTREL: return QStringLiteral("Reloc might modify .text");
        case DT_JMPREL: return QStringLiteral("PLT reloc address");
        case DT_BIND_NOW: return QStringLiteral("BIND_NOW");
        case DT_INIT_ARRAY: return QStringLiteral("Init function address array");
        case DT_FINI_ARRAY: return QStringLiteral("Termination function address array");
        case DT_INIT_ARRAYSZ: return QStringLiteral("Init function address array size");
        case DT_FINI_ARRAYSZ: return QStringLiteral("Termination function address array size");
        case DT_RUNPATH: return QStringLiteral("RUNPATH");
        case DT_FLAGS: return QStringLiteral("Flags");
        case DT_PREINIT_ARRAY: return QStringLiteral("Preinit function address array");
        case DT_PREINIT_ARRAYSZ: return QStringLiteral("Preinit function address array size");
#if 0
        #define DT_GNU_PRELINKED 0x6ffffdf5     /* Prelinking timestamp */
        #define DT_GNU_CONFLICTSZ 0x6ffffdf6    /* Size of conflict section */
        #define DT_GNU_LIBLISTSZ 0x6ffffdf7     /* Size of library list */
        #define DT_CHECKSUM     0x6ffffdf8
        #define DT_PLTPADSZ     0x6ffffdf9
        #define DT_MOVEENT      0x6ffffdfa
        #define DT_MOVESZ       0x6ffffdfb
        #define DT_FEATURE_1    0x6ffffdfc      /* Feature selection (DTF_*).  */
        #define DT_POSFLAG_1    0x6ffffdfd      /* Flags for DT_* entries, effecting the following DT_* entry.  */
#endif
        case DT_SYMINSZ: return QStringLiteral("Syminfo table size");
        case DT_SYMINENT: return QStringLiteral("Syminfo table entry size");
        case DT_GNU_HASH: return QStringLiteral("GNU hash table address");
        case DT_TLSDESC_PLT: return QStringLiteral("TLS description PLT");
        case DT_TLSDESC_GOT: return QStringLiteral("TLS description GOT");
        case DT_GNU_CONFLICT: return QStringLiteral("GNU conflict section");
        case DT_GNU_LIBLIST: return QStringLiteral("GNU library list");
#if 0
        #define DT_CONFIG       0x6ffffefa      /* Configuration information.  */
        #define DT_DEPAUDIT     0x6ffffefb      /* Dependency auditing.  */
        #define DT_AUDIT        0x6ffffefc      /* Object auditing.  */
        #define DT_PLTPAD       0x6ffffefd      /* PLT padding.  */
        #define DT_MOVETAB      0x6ffffefe      /* Move table.  */
        #define DT_SYMINFO      0x6ffffeff      /* Syminfo table.  */
#endif
        case DT_VERSYM: return QStringLiteral("GNU version symbol");
        case DT_RELACOUNT: return QStringLiteral("Rela count");
        case DT_RELCOUNT: return QStringLiteral("Rel count");
        case DT_FLAGS_1: return QStringLiteral("State flags");
        case DT_VERDEF: return QStringLiteral("Address of version definition table");
        case DT_VERDEFNUM: return QStringLiteral("Number of version definitions");
        case DT_VERNEED: return QStringLiteral("Needed versions table address");
        case DT_VERNEEDNUM: return QStringLiteral("Number of needed versions");
        default:
            return QObject::tr("unknown (%1)").arg(tag());
    }

    return QString();
}

bool ElfDynamicEntry::isStringValue() const
{
    return tag() == DT_NEEDED
        || tag() == DT_SONAME
        || tag() == DT_RPATH
        || tag() == DT_RUNPATH
    ; // TODO complete
}

const char* ElfDynamicEntry::stringValue() const
{
    assert(isStringValue());
    return dynamicSection()->linkedSection<ElfStringTableSection>()->string(value());
}

bool ElfDynamicEntry::isAddress() const
{
    switch (tag()) {
        case DT_PLTGOT:
        case DT_HASH:
        case DT_STRTAB:
        case DT_SYMTAB:
        case DT_RELA:
        case DT_INIT:
        case DT_FINI:
        case DT_REL:
        case DT_JMPREL:
        case DT_INIT_ARRAY:
        case DT_FINI_ARRAY:
        case DT_PREINIT_ARRAY:
        case DT_VERSYM:
        case DT_VERDEF:
        case DT_VERNEED:
            return true;
    }

    return tag() >= DT_ADDRRNGLO && tag() <= DT_ADDRRNGHI;
}
