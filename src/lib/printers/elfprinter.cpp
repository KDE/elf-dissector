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
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "elfprinter.h"
#include "printerutils_p.h"

#include <elf.h>

#include <QByteArray>

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
#ifdef EM_AARCH64
        M(AARCH64)
#endif
    }
    return QByteArray("Unknown machine type (" ) + QByteArray::number(machineType) + ')';
}

static const LookupTableEntry<uint32_t> section_type_table[] {
    { SHT_NULL, "null" },
    { SHT_PROGBITS, "program data" },
    { SHT_SYMTAB, "symbol table" },
    { SHT_STRTAB, "string table" },
    { SHT_RELA, "relocation entries with addends" },
    { SHT_HASH, "symbol hash table" },
    { SHT_DYNAMIC, "dynamic linking information" },
    { SHT_NOTE, "notes" },
    { SHT_NOBITS, "bss" },
    { SHT_REL, "relocation entries, no addends" },
    { SHT_SHLIB, "reserved" },
    { SHT_DYNSYM, "dynamic linker symbol table" },
    { SHT_INIT_ARRAY, "array of constructors" },
    { SHT_FINI_ARRAY, "array of destructors" },
    { SHT_PREINIT_ARRAY, "array of preconstructors" },
    { SHT_GROUP, "section group" },
    { SHT_SYMTAB_SHNDX, "extended section indices" },

    { SHT_GNU_ATTRIBUTES, "GNU object attributes" },
    { SHT_GNU_HASH, "GNU-style hash table" },
    { SHT_GNU_LIBLIST, "GNU prelink library list" },
    { SHT_CHECKSUM, "checksum for DSO content" },
    { SHT_GNU_verdef, "GNU version definition" },
    { SHT_GNU_verneed, "GNU version needs" },
    { SHT_GNU_versym, "GNU version symbol table" }
};

QByteArray ElfPrinter::sectionType(uint32_t sectionType)
{
    return lookupLabel(sectionType, section_type_table);
}

static const LookupTableEntry<uint64_t> section_flags_table[] {
    { SHF_WRITE, "writable" },
    { SHF_ALLOC, "occupies memory during execution" },
    { SHF_EXECINSTR, "executable" },
    { SHF_MERGE, "might be merged" },
    { SHF_STRINGS, "contains nul-terminated strings" },
    { SHF_INFO_LINK, "sh_info contains SHT index" },
    { SHF_LINK_ORDER, "preserve order after combining" },
    { SHF_OS_NONCONFORMING, "non-standard OS specific handling required" },
    { SHF_GROUP, "group member" },
    { SHF_TLS, "holds thread-local data" }
};

QByteArray ElfPrinter::sectionFlags(uint64_t flags)
{
    return lookupFlags(flags, section_flags_table);
}

static const LookupTableEntry<uint8_t> os_abi_table[] {
    { ELFOSABI_SYSV, "UNIX System V ABI" },
    { ELFOSABI_HPUX, "HP-UX" },
    { ELFOSABI_NETBSD, "NetBSD" },
    { ELFOSABI_GNU, "Object uses GNU ELF extensions" },
    { ELFOSABI_SOLARIS, "Sun Solaris" },
    { ELFOSABI_AIX, "IBM AIX" },
    { ELFOSABI_IRIX, "SGI Irix" },
    { ELFOSABI_FREEBSD, "FreeBSD" },
    { ELFOSABI_TRU64, "Compaq TRU64 UNIX" },
    { ELFOSABI_MODESTO, "Novell Modesto" },
    { ELFOSABI_OPENBSD, "OpenBSD" },
    { ELFOSABI_ARM_AEABI, "ARM EABI" },
    { ELFOSABI_ARM, "ARM" },
    { ELFOSABI_STANDALONE, "Standalone (embedded) application" }
};

QByteArray ElfPrinter::osAbi(uint8_t abi)
{
    return lookupLabel(abi, os_abi_table);
}

static const LookupTableEntry<uint32_t> segment_type_table[] {
    { PT_NULL, "Program header table entry unused" },
    { PT_LOAD, "Loadable program segment" },
    { PT_DYNAMIC, "Dynamic linking information" },
    { PT_INTERP, "Program interpreter" },
    { PT_NOTE, "Auxiliary information" },
    { PT_SHLIB, "Reserved" },
    { PT_PHDR, "Entry for header table itself" },
    { PT_TLS, "Thread-local storage segment" },
    { PT_NUM, "Number of defined types" },
    { PT_GNU_EH_FRAME, "GCC .eh_frame_hdr segment" },
    { PT_GNU_STACK, "Indicates stack executability" },
    { PT_GNU_RELRO, "Read-only after relocation" },
    { PT_SUNWBSS, "Sun Specific segment" },
    { PT_SUNWSTACK, "Stack segment" }
};

QByteArray ElfPrinter::segmentType(uint32_t segmentType)
{
    return lookupLabel(segmentType, segment_type_table);
}

static const LookupTableEntry<uint32_t> segment_flags_table[] {
    { PF_X, "executable" },
    { PF_W, "writable" },
    { PF_R, "readable" }
};

QByteArray ElfPrinter::segmentFlags(uint32_t flags)
{
    return lookupFlags(flags, segment_flags_table);
}
