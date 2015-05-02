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

static const RelocType reloc_types_i386[] {
    RT(R_386_NONE, "No reloc"),
    RT(R_386_32, "Direct 32 bit"),
    RT(R_386_PC32, "PC relative 32 bit"),
    RT(R_386_GOT32, "32 bit GOT entry"),
    RT(R_386_PLT32, "32 bit PLT address"),
    RT(R_386_COPY, "Copy symbol at runtime"),
    RT(R_386_GLOB_DAT, "Create GOT entry"),
    RT(R_386_JMP_SLOT, "Create PLT entry"),
    RT(R_386_RELATIVE, "Adjust by program base"),
    RT(R_386_GOTOFF, "32 bit offset to GOT"),
    RT(R_386_GOTPC, "32 bit PC relative offset to GOT"),
    RT(R_386_32PLT, ""),
    RT(R_386_TLS_TPOFF, "Offset in static TLS block"),
    RT(R_386_TLS_IE, "Address of GOT entry for static TLS block offset"),
    RT(R_386_TLS_GOTIE, "GOT entry for static TLS block offset"),
    RT(R_386_TLS_LE, "Offset relative to static TLS block"),
    RT(R_386_TLS_GD, "Direct 32 bit for GNU version of general dynamic thread local data"),
    RT(R_386_TLS_LDM, "Direct 32 bit for GNU version of local dynamic thread local data in LE code"),
    RT(R_386_16, ""),
    RT(R_386_PC16, ""),
    RT(R_386_8, ""),
    RT(R_386_PC8, ""),
    RT(R_386_TLS_GD_32, "Direct 32 bit for general dynamic thread local data"),
    RT(R_386_TLS_GD_PUSH, "Tag for pushl in GD TLS code"),
    RT(R_386_TLS_GD_CALL, "Relocation for call to __tls_get_addr()"),
    RT(R_386_TLS_GD_POP, "Tag for popl in GD TLS code"),
    RT(R_386_TLS_LDM_32, "Direct 32 bit for local dynamic thread local data in LE code"),
    RT(R_386_TLS_LDM_PUSH, "Tag for pushl in LDM TLS code"),
    RT(R_386_TLS_LDM_CALL, "Relocation for call to __tls_get_addr() in LDM code"),
    RT(R_386_TLS_LDM_POP, "Tag for popl in LDM TLS code"),
    RT(R_386_TLS_LDO_32, "Offset relative to TLS block"),
    RT(R_386_TLS_IE_32, "GOT entry for negated static TLS block offset"),
    RT(R_386_TLS_LE_32, "Negated offset relative to static TLS block"),
    RT(R_386_TLS_DTPMOD32, "ID of module containing symbol"),
    RT(R_386_TLS_DTPOFF32, "Offset in TLS block"),
    RT(R_386_TLS_TPOFF32, "Negated offset in static TLS block"),
    RT(R_386_SIZE32, "32-bit symbol size"),
    RT(R_386_TLS_GOTDESC, "GOT offset for TLS descriptor."),
    RT(R_386_TLS_DESC_CALL, "Marker of call through TLS descriptor for relaxation."),
    RT(R_386_TLS_DESC, "TLS descriptor containing pointer to code and to argument, returning the TLS offset for the symbol."),
    RT(R_386_IRELATIVE, "Adjust indirectly by program base")
};

static const RelocType reloc_types_arm[] {
    RT(R_ARM_NONE, "No reloc"),
    RT(R_ARM_PC24, "Deprecated PC relative 26 bit branch"),
    RT(R_ARM_ABS32, "Direct 32 bit"),
    RT(R_ARM_REL32, "PC relative 32 bit"),
    RT(R_ARM_PC13, "PC13"),
    RT(R_ARM_ABS16, "Direct 16 bit"),
    RT(R_ARM_ABS12, "Direct 12 bit"),
    RT(R_ARM_THM_ABS5, "Direct & 0x7C (LDR, STR)"),
    RT(R_ARM_ABS8, "Direct 8 bit"),
    RT(R_ARM_SBREL32, "SBREL32"),
    RT(R_ARM_THM_PC22, "PC relative 24 bit (Thumb32 BL)"),
    RT(R_ARM_THM_PC8, "PC relative & 0x3FC (Thumb16 LDR, ADD, ADR)"),
    RT(R_ARM_AMP_VCALL9, "VCALL9"),
    RT(R_ARM_SWI24, "Obsolete static relocation"),
    RT(R_ARM_TLS_DESC, "Dynamic relocation"),
    RT(R_ARM_THM_SWI8, "SWI8"),
    RT(R_ARM_XPC25, "XPC25"),
    RT(R_ARM_THM_XPC22, "XPC22 (Thumb)"),
    RT(R_ARM_TLS_DTPMOD32, "ID of module containing symbol"),
    RT(R_ARM_TLS_DTPOFF32, "Offset in TLS block"),
    RT(R_ARM_TLS_TPOFF32, "Offset in static TLS block"),
    RT(R_ARM_COPY, "Copy symbol at runtime"),
    RT(R_ARM_GLOB_DAT, "Create GOT entry"),
    RT(R_ARM_JUMP_SLOT, "Create PLT entry"),
    RT(R_ARM_RELATIVE, "Adjust by program base"),
    RT(R_ARM_GOTOFF, "32 bit offset to GOT"),
    RT(R_ARM_GOTPC, "32 bit PC relative offset to GOT"),
    RT(R_ARM_GOT32, "32 bit GOT entry"),
    RT(R_ARM_PLT32, "Deprecated, 32 bit PLT address"),
    RT(R_ARM_CALL, "PC relative 24 bit (BL, BLX)"),
    RT(R_ARM_JUMP24, "PC relative 24 bit (B, BL<cond>)"),
    RT(R_ARM_THM_JUMP24, "PC relative 24 bit (Thumb32 B.W)"),
    RT(R_ARM_BASE_ABS, "Adjust by program base"),
    RT(R_ARM_ALU_PCREL_7_0, "ALU RCREL 7 0"),
    RT(R_ARM_ALU_PCREL_15_8, "ALU PCREL 15 8"),
    RT(R_ARM_ALU_PCREL_23_15, "ALU RCREL 23 15"),
    RT(R_ARM_LDR_SBREL_11_0, "Program base relative (LDR 11 0)"),
    RT(R_ARM_ALU_SBREL_19_12, "Program base relative (ALU 19 12)"),
    RT(R_ARM_ALU_SBREL_27_20, "Program base relative (ALU 27 20)"),
    RT(R_ARM_TARGET1, "TARGET1"),
    RT(R_ARM_SBREL31, "Program base relative"),
    RT(R_ARM_V4BX, "V4BX"),
    RT(R_ARM_TARGET2, "TARGET2"),
    RT(R_ARM_PREL31, "32 bit PC relative"),
    RT(R_ARM_MOVW_ABS_NC, "Direct 16-bit (MOVW)"),
    RT(R_ARM_MOVT_ABS, "Direct high 16-bit (MOVT)"),
    RT(R_ARM_MOVW_PREL_NC, "PC relative 16-bit (MOVW)"),
    RT(R_ARM_MOVT_PREL, "PC relative (MOVT)"),
    RT(R_ARM_THM_MOVW_ABS_NC, "Direct 16 bit (Thumb32 MOVW)"),
    RT(R_ARM_THM_MOVT_ABS, "Direct high 16 bit (Thumb32 MOVT)"),
    RT(R_ARM_THM_MOVW_PREL_NC, "PC relative 16 bit (Thumb32 MOVW)"),
    RT(R_ARM_THM_MOVT_PREL, "PC relative high 16 bit (Thumb32 MOVT)"),
    RT(R_ARM_THM_JUMP19, "PC relative 20 bit (Thumb32 B<cond>.W)"),
    RT(R_ARM_THM_JUMP6, "PC relative X & 0x7E (Thumb16 CBZ, CBNZ)"),
    RT(R_ARM_THM_ALU_PREL_11_0, "PC relative 12 bit (Thumb32 ADR.W)"),
    RT(R_ARM_THM_PC12, "PC relative 12 bit (Thumb32 LDR{D,SB,H,SH})"),
    RT(R_ARM_ABS32_NOI, "Direct 32-bit"),
    RT(R_ARM_REL32_NOI, "PC relative 32-bit"),
    RT(R_ARM_ALU_PC_G0_NC, "PC relative (ADD, SUB)"),
    RT(R_ARM_ALU_PC_G0, "PC relative (ADD, SUB)"),
    RT(R_ARM_ALU_PC_G1_NC, "PC relative (ADD, SUB)"),
    RT(R_ARM_ALU_PC_G1, "PC relative (ADD, SUB)"),
    RT(R_ARM_ALU_PC_G2, "PC relative (ADD, SUB)"),
    RT(R_ARM_LDR_PC_G1, "PC relative (LDR,STR,LDRB,STRB)"),
    RT(R_ARM_LDR_PC_G2, "PC relative (LDR,STR,LDRB,STRB)"),
    RT(R_ARM_LDRS_PC_G0, "PC relative (STR{D,H}, LDR{D,SB,H,SH})"),
    RT(R_ARM_LDRS_PC_G1, "PC relative (STR{D,H}, LDR{D,SB,H,SH})"),
    RT(R_ARM_LDRS_PC_G2, "PC relative (STR{D,H}, LDR{D,SB,H,SH})"),
    RT(R_ARM_LDC_PC_G0, "PC relative (LDC, STC)"),
    RT(R_ARM_LDC_PC_G1, "PC relative (LDC, STC)"),
    RT(R_ARM_LDC_PC_G2, "PC relative (LDC, STC)"),
    RT(R_ARM_ALU_SB_G0_NC, "Program base relative (ADD,SUB)"),
    RT(R_ARM_ALU_SB_G0, "Program base relative (ADD,SUB)"),
    RT(R_ARM_ALU_SB_G1_NC, "Program base relative (ADD,SUB)"),
    RT(R_ARM_ALU_SB_G1, "Program base relative (ADD,SUB)"),
    RT(R_ARM_ALU_SB_G2, "Program base relative (ADD,SUB)"),
    RT(R_ARM_LDR_SB_G0, "Program base relative (LDR, STR, LDRB, STRB)"),
    RT(R_ARM_LDR_SB_G1, "Program base relative (LDR, STR, LDRB, STRB)"),
    RT(R_ARM_LDR_SB_G2, "Program base relative (LDR, STR, LDRB, STRB)"),
    RT(R_ARM_LDRS_SB_G0, "Program base relative (LDR, STR, LDRB, STRB)"),
    RT(R_ARM_LDRS_SB_G1, "Program base relative (LDR, STR, LDRB, STRB)"),
    RT(R_ARM_LDRS_SB_G2, "Program base relative (LDR, STR, LDRB, STRB)"),
    RT(R_ARM_LDC_SB_G0, "Program base relative (LDC,STC)"),
    RT(R_ARM_LDC_SB_G1, "Program base relative (LDC,STC)"),
    RT(R_ARM_LDC_SB_G2, "Program base relative (LDC,STC)"),
    RT(R_ARM_MOVW_BREL_NC, "Program base relative 16 bit (MOVW)"),
    RT(R_ARM_MOVT_BREL, "Program base relative high 16 bit (MOVT)"),
    RT(R_ARM_MOVW_BREL, "Program base relative 16 bit (MOVW)"),
    RT(R_ARM_THM_MOVW_BREL_NC, "Program base relative 16 bit (Thumb32 MOVW)"),
    RT(R_ARM_THM_MOVT_BREL, "Program base relative high 16 bit (Thumb32 MOVT)"),
    RT(R_ARM_THM_MOVW_BREL, "Program base relative 16 bit (Thumb32 MOVW)"),
    RT(R_ARM_TLS_GOTDESC, "TLS GOTDESC"),
    RT(R_ARM_TLS_CALL, "TLS CALL"),
    RT(R_ARM_TLS_DESCSEQ, "TLS relaxation"),
    RT(R_ARM_THM_TLS_CALL, "TLS CALL (Thumb)"),
    RT(R_ARM_PLT32_ABS, "PLT32 ABS"),
    RT(R_ARM_GOT_ABS, "GOT entry"),
    RT(R_ARM_GOT_PREL, "PC relative GOT entry"),
    RT(R_ARM_GOT_BREL12, "GOT entry relative to GOT origin (LDR)"),
    RT(R_ARM_GOTOFF12, "12 bit, GOT entry relative to GOT origin (LDR, STR)"),
    RT(R_ARM_GOTRELAX, "GOTRELAX"),
    RT(R_ARM_GNU_VTENTRY, "GNU VTENTRY"),
    RT(R_ARM_GNU_VTINHERIT, "GNU VTINHERIT"),
    RT(R_ARM_THM_PC11, "PC relative & 0xFFE (Thumb16 B)"),
    RT(R_ARM_THM_PC9, "PC relative & 0x1FE (Thumb16 B/B<cond>)"),
    RT(R_ARM_TLS_GD32, "PC-rel 32 bit for global dynamic thread local data"),
    RT(R_ARM_TLS_LDM32, "PC-rel 32 bit for local dynamic thread local data"),
    RT(R_ARM_TLS_LDO32, "32 bit offset relative to TLS block"),
    RT(R_ARM_TLS_IE32, "PC-rel 32 bit for GOT entry of static TLS block offset"),
    RT(R_ARM_TLS_LE32, "32 bit offset relative to static TLS block"),
    RT(R_ARM_TLS_LDO12, "12 bit relative to TLS block (LDR, STR)"),
    RT(R_ARM_TLS_LE12, "12 bit relative to static TLS block (LDR, STR)"),
    RT(R_ARM_TLS_IE12GP, "12 bit GOT entry relative to GOT origin (LDR)"),
    RT(R_ARM_ME_TOO, "ME TOO"),
    RT(R_ARM_THM_TLS_DESCSEQ, "TLS DESCSEQ (Thumb)"),
    RT(R_ARM_THM_TLS_DESCSEQ16, "TLS DESCSEQ16 (Thumb)"),
    RT(R_ARM_THM_TLS_DESCSEQ32, "TLS DESCSEQ32 (Thumb)"),
    RT(R_ARM_THM_GOT_BREL12, "GOT entry relative to GOT origin, 12 bit (Thumb32 LDR)"),
    RT(R_ARM_IRELATIVE, "IRELATIVE"),
    RT(R_ARM_RXPC25, "RXPC25"),
    RT(R_ARM_RSBREL32, "RSBREL32"),
    RT(R_ARM_THM_RPC22, "RPC22 (Thumb)"),
    RT(R_ARM_RREL32, "RREL32"),
    RT(R_ARM_RABS22, "RABS22"),
    RT(R_ARM_RPC24, "RPC24"),
    RT(R_ARM_RBASE, "RBASE")
};

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
    { EM_386, reloc_types_i386, sizeof(reloc_types_i386) / sizeof(RelocType) },
    { EM_ARM, reloc_types_arm, sizeof(reloc_types_arm) / sizeof(RelocType) },
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
