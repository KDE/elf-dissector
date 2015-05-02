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

#include "elfprinter.h"
#include "printerutils_p.h"

#include <elf.h>

#include <QByteArray>
#include <QList>

static const LookupTableEntry<uint16_t> file_type_table[] {
    { ET_NONE, "none" },
    { ET_REL, "relocatable file" },
    { ET_EXEC, "executable file" },
    { ET_DYN, "shared object file" },
    { ET_CORE, "core file" }
};

QByteArray ElfPrinter::fileType(uint16_t fileType)
{
    return lookupLabel(fileType, file_type_table);
}

QByteArray ElfPrinter::machine(uint16_t machineType)
{
    #define M(x) case EM_ ## x: return QByteArray::fromRawData(#x, strlen(#x));
    switch (machineType) {
        M(NONE)
        M(386)
        M(ARM)
        M(X86_64)
        M(AVR)
        M(AARCH64)
    }
    return QByteArray("Unknown machine type (" ) + QByteArray::number(machineType) + ')';
}

QByteArray ElfPrinter::sectionType(uint32_t sectionType)
{
    switch (sectionType) {
        case SHT_NULL: return "null";
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

    }

    return QByteArray("unknown section type (") + QByteArray::number(sectionType) + ')';
}

QByteArray ElfPrinter::sectionFlags(uint64_t flags)
{
    QByteArrayList s;
    if (flags & SHF_WRITE) s.push_back("writable");
    if (flags & SHF_ALLOC) s.push_back("occupies memory during execution");
    if (flags & SHF_EXECINSTR) s.push_back("executable");
    if (flags & SHF_MERGE) s.push_back("might be merged");
    if (flags & SHF_STRINGS) s.push_back("contains nul-terminated strings");
    if (flags & SHF_INFO_LINK) s.push_back("sh_info contains SHT index");
    if (flags & SHF_LINK_ORDER) s.push_back("preserve order after combining");
    if (flags & SHF_OS_NONCONFORMING) s.push_back("non-standard OS specific handling required");
    if (flags & SHF_GROUP) s.push_back("group member");
    if (flags & SHF_TLS) s.push_back("holds thread-local data");
    if (s.isEmpty())
        return "none";
    return s.join(", ");
}

