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

#include "relocationprinter.h"

#include <elf/elfrelocationentry.h>
#include <elf/elfrelocationsection.h>
#include <elf/elffile.h>
#include <elf/elfheader.h>

#include <QByteArray>

#include <elf.h>

struct RelocType
{
    uint32_t id;
    const char* label;
    const char* desc;
};

#define RT(type, desc) { type, #type, desc }

static const RelocType reloc_types_x86_64[] {
    RT(R_X86_64_NONE, "No reloc"),
    RT(R_X86_64_64, "Direct 64 bit "),
    RT(R_X86_64_PC32, "PC relative 32 bit signed"),
    RT(R_X86_64_GOT32, "32 bit GOT entry"),
    RT(R_X86_64_PLT32, "32 bit PLT address"),
    RT(R_X86_64_COPY, "Copy symbol at runtime"),
    RT(R_X86_64_GLOB_DAT, "Create GOT entry"),
    RT(R_X86_64_JUMP_SLOT, "Create PLT entry"),
    RT(R_X86_64_RELATIVE, "Adjust by program base"),
    RT(R_X86_64_GOTPCREL, "32 bit signed PC relative offset to GOT"),
    RT(R_X86_64_32, "Direct 32 bit zero extended"),
    RT(R_X86_64_32S, "Direct 32 bit sign extended"),
    RT(R_X86_64_16, "Direct 16 bit zero extended"),
    RT(R_X86_64_PC16, "16 bit sign extended pc relative"),
    RT(R_X86_64_8, "Direct 8 bit sign extended "),
    RT(R_X86_64_PC8, "8 bit sign extended pc relative"),
    RT(R_X86_64_DTPMOD64, "ID of module containing symbol"),
    RT(R_X86_64_DTPOFF64, "Offset in module's TLS block"),
    RT(R_X86_64_TPOFF64, "Offset in initial TLS block"),
    RT(R_X86_64_TLSGD, "32 bit signed PC relative offset to two GOT entries for GD symbol"),
    RT(R_X86_64_TLSLD, "32 bit signed PC relative offset to two GOT entries for LD symbol"),
    RT(R_X86_64_DTPOFF32, "Offset in TLS block"),
    RT(R_X86_64_GOTTPOFF, "32 bit signed PC relative offset to GOT entry for IE symbol"),
    RT(R_X86_64_TPOFF32, "Offset in initial TLS block"),
    RT(R_X86_64_PC64, "PC relative 64 bit"),
    RT(R_X86_64_GOTOFF64, "64 bit offset to GOT"),
    RT(R_X86_64_GOTPC32, "32 bit signed pc relative offset to GOT"),
    RT(R_X86_64_GOT64, "64-bit GOT entry offset"),
    RT(R_X86_64_GOTPCREL64, "64-bit PC relative offset to GOT entry"),
    RT(R_X86_64_GOTPC64, "64-bit PC relative offset to GOT"),
    RT(R_X86_64_GOTPLT64, "like GOT64, says PLT entry needed"),
    RT(R_X86_64_PLTOFF64, "64-bit GOT relative offset to PLT entry"),
    RT(R_X86_64_SIZE32, "Size of symbol plus 32-bit addend"),
    RT(R_X86_64_SIZE64, "Size of symbol plus 64-bit addend"),
    RT(R_X86_64_GOTPC32_TLSDESC, "GOT offset for TLS descriptor"),
    RT(R_X86_64_TLSDESC_CALL, "Marker for call through TLS descriptor"),
    RT(R_X86_64_TLSDESC, "TLS descriptor. "),
    RT(R_X86_64_IRELATIVE, "Adjust indirectly by program base"),
    RT(R_X86_64_RELATIVE64, "64-bit adjust by program base")
};

#undef RT

struct RelocTypeRepository
{
    int machine;
    const RelocType* typeInfos;
    int typeInfosSize;
};

static const RelocTypeRepository reloc_type_repository[] {
    { EM_X86_64, reloc_types_x86_64, sizeof(reloc_types_x86_64) / sizeof(RelocType) }
};

static const int reloc_type_repository_size = sizeof(reloc_type_repository) / sizeof(RelocTypeRepository);

static const RelocType* relocTypeInfo(ElfRelocationEntry* entry)
{
    // TODO can be optimized, tables are sorted
    for (int i = 0; i < reloc_type_repository_size; ++i) {
        if (reloc_type_repository[i].machine != entry->relocationTable()->file()->header()->machine())
            continue;
        for (int j = 0; j < reloc_type_repository[i].typeInfosSize; ++j) {
            if (reloc_type_repository[i].typeInfos[j].id == entry->type())
                return &reloc_type_repository[i].typeInfos[j];
        }
    }

    return nullptr;
}

namespace RelocationPrinter {

QByteArray label(ElfRelocationEntry* entry)
{
    const auto info = relocTypeInfo(entry);
    if (!info)
        return QByteArray("unknown relocation type ") + QByteArray::number(entry->type());
    return QByteArray::fromRawData(info->label, strlen(info->label));
}

QByteArray description(ElfRelocationEntry* entry)
{
    const auto info = relocTypeInfo(entry);
    if (!info)
        return QByteArray("unknown relocation type ") + QByteArray::number(entry->type());
    return QByteArray::fromRawData(info->desc, strlen(info->desc));
}

}
