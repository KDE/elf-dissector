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

#include "disassembler.h"

#include <elf/elfsymboltableentry.h>
#include <elf/elfsymboltablesection.h>
#include <elf/elffile.h>
#include <elf/elfheader.h>
#include <elf/elfpltentry.h>
#include <elf/elfpltsection.h>
#include <elf/elfgotsection.h>
#include <elf/elfrelocationentry.h>
#include <dwarf/dwarfinfo.h>
#include <dwarf/dwarfaddressranges.h>
#include <dwarf/dwarfcudie.h>
#include <dwarf/dwarfline.h>

#include <QDebug>
#include <QString>

#include <cassert>
#include <cstdarg>

#define PACKAGE "elf-dissector"
#define PACKAGE_VERSION "0.0.1"
#include <dis-asm.h>
#include <elf.h>
#include <stdio.h>

static int qstring_printf(void *data, const char *format, ...)
{
    QString buffer;
    va_list args;
    va_start(args, format);
    buffer.vsprintf(format, args);
    va_end(args);

    QString *s = static_cast<QString*>(data);
    s->append(buffer);
    return buffer.size();
}

static void print_address(bfd_vma addr, struct disassemble_info *info)
{
    const auto disasm = static_cast<Disassembler*>(info->application_data);
    assert(disasm);

    // TODO handle relocations/PLT/etc

    (*info->fprintf_func) (info->stream, "0x%lx", addr);

    const uint64_t targetAddr = disasm->baseAddress() + addr;
    const auto target = disasm->file()->symbolTable()->entryWithValue(targetAddr);
    if (target) {
        auto s = static_cast<QString*>(info->stream);
        s->append(" (");
        s->append(disasm->printSymbol(target));
        s->append(')');
        return;
    }

    const auto secIdx = disasm->file()->indexOfSectionWithVirtualAddress(targetAddr);
    if (secIdx < 0)
        return;

    const auto section = disasm->file()->section<ElfSection>(secIdx);
    assert(section);

    const auto pltSection = dynamic_cast<ElfPltSection*>(section);
    if (pltSection) {
        const auto pltEntry = pltSection->entry((targetAddr - section->header()->virtualAddress()) / section->header()->entrySize());
        assert(pltEntry);
        auto s = static_cast<QString*>(info->stream);
        s->append(" (");
        s->append(disasm->printPltEntry(pltEntry));
        s->append(')');
        return;
    }

    const auto gotSection = dynamic_cast<ElfGotSection*>(section);
    if (gotSection) {
        const auto gotEntry = gotSection->entry((targetAddr - section->header()->virtualAddress()) / disasm->file()->addressSize());
        assert(gotEntry);
        auto s = static_cast<QString*>(info->stream);
        s->append(" (");
        s->append(disasm->printGotEntry(gotEntry));
        s->append(')');
        return;
    }

    (*info->fprintf_func) (info->stream, " (%s + 0x%lx)", section->header()->name(), targetAddr - section->header()->virtualAddress());
}

Disassembler::~Disassembler() = default;

QString Disassembler::disassemble(ElfSection* section)
{
    m_file = section->file();
    m_baseAddress = section->header()->virtualAddress();
    return disassemble(section->rawData(), section->size());
}

QString Disassembler::disassemble(ElfSymbolTableEntry* entry)
{
    m_file = entry->symbolTable()->file();
    m_baseAddress = entry->value();
    return disassemble(entry->data(), entry->size());

}

QString Disassembler::disassemble(ElfPltEntry* entry)
{
    m_file = entry->section()->file();
    m_baseAddress = entry->section()->header()->virtualAddress() + entry->index() * entry->size();
    return disassemble(entry->rawData(), entry->size());
}

QString Disassembler::disassemble(const unsigned char* data, uint64_t size)
{
    QString result;
    disassembler_ftype disassemble_fn;
    disassemble_info info;
    INIT_DISASSEMBLE_INFO(info, &result, qstring_printf);

    info.application_data = this;
    info.flavour = bfd_target_elf_flavour;
    info.endian = m_file->byteOrder() == ELFDATA2LSB ? BFD_ENDIAN_LITTLE : BFD_ENDIAN_BIG;
    switch (m_file->header()->machine()) {
#if defined(__x86_64__) || defined(__i386__)
        case EM_386:
            info.arch = bfd_arch_i386;
            info.mach = bfd_mach_i386_i386;
            disassemble_fn = print_insn_i386;
            break;
        case EM_X86_64:
            info.arch = bfd_arch_i386;
            info.mach = bfd_mach_x86_64;
            disassemble_fn = print_insn_i386;
            break;
#endif
#if defined(__arm__)
        case EM_ARM:
            info.arch = bfd_arch_arm;
            info.mach = bfd_mach_arm_unknown;
            if (info.endian == BFD_ENDIAN_LITTLE)
                disassemble_fn = print_insn_little_arm;
            else
                disassemble_fn = print_insn_big_arm;
            break;
#endif
        default:
            qWarning() << "Unsupported architecture!";
            return {};
    }

    info.buffer = const_cast<bfd_byte*>(data);
    info.buffer_length = size;
    info.buffer_vma = 0;
    info.print_address_func = print_address;

    uint32_t bytes = 0;
    while (bytes < size) {
        auto line = lineForAddress(baseAddress() + bytes);
        if (!line.isNull())
            result += printSourceLine(line) + "<br/>";
        result += QStringLiteral("%1: ").arg(bytes, 8, 10);
        bytes += (*disassemble_fn)(bytes, &info);
        result += QLatin1String("<br/>");
    }

    return result;
}

ElfFile* Disassembler::file() const
{
    return m_file;
}

uint64_t Disassembler::baseAddress() const
{
    return m_baseAddress;
}

QString Disassembler::printSymbol(ElfSymbolTableEntry* entry) const
{
    return QLatin1String(entry->name());
}

QString Disassembler::printGotEntry(ElfGotEntry* entry) const
{
    const auto reloc = entry->relocation();
    const auto sym = reloc ? reloc->symbol() : nullptr;
    if (sym)
        return sym->name() + QStringLiteral("@got");
    return entry->section()->header()->name() + QStringLiteral(" + 0x") + QString::number(entry->index() * entry->section()->file()->addressSize());
}

QString Disassembler::printPltEntry(ElfPltEntry* entry) const
{
    const auto gotEntry = entry->gotEntry();
    const auto reloc = gotEntry ? gotEntry->relocation() : nullptr;
    const auto sym = reloc ? reloc->symbol() : nullptr;
    if (sym)
        return sym->name() + QStringLiteral("@plt");
    return entry->section()->header()->name() + QStringLiteral(" + 0x") + QString::number(entry->index() * entry->section()->header()->entrySize());
}

DwarfLine Disassembler::lineForAddress(uint64_t addr)
{
    if (!file()->dwarfInfo())
        return {};

    auto cu = file()->dwarfInfo()->compilationUnitForAddress(addr);
    if (!cu)
        return {};
    return cu->lineForAddress(addr);
}

QString Disassembler::printSourceLine(DwarfLine line)
{
    assert(!line.isNull());
    auto cu = file()->dwarfInfo()->compilationUnitForAddress(line.address());
    assert(cu);

    QString s;
    s += "<i>Source: " + QString::fromUtf8(cu->sourceFileForLine(line));
    s += ':' + QString::number(line.line()) + "</i>";
    return s;
}
