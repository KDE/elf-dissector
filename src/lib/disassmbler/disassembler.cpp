/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "disassembler.h"
#include "config-elf-dissector.h"

#include <elf/elfsymboltableentry.h>
#include <elf/elfsymboltablesection.h>
#include <elf/elffile.h>
#include <elf/elfheader.h>
#include <elf/elfpltentry.h>
#include <elf/elfpltsection.h>
#include <elf/elfgotsection.h>
#include <elf/elfrelocationentry.h>
#if HAVE_DWARF
#include <dwarf/dwarfinfo.h>
#include <dwarf/dwarfaddressranges.h>
#include <dwarf/dwarfcudie.h>
#include <dwarf/dwarfline.h>
#endif

#include <QDebug>
#include <QString>
#include <QUrl>

#include <cassert>
#include <cstdarg>

#include <ansidecl.h>

#define PACKAGE "elf-dissector"
#define PACKAGE_VERSION "0.0.1"
#include <dis-asm.h>
#include <elf.h>
#include <stdio.h>

#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 29)
    // in binutils 2.29 print_insn_i386 disappeared from the dis-asm.h header,
    // not sure what the proper replacement for it is, so define it here
    // See commit 88c1242dc0a1e1ab582a65ea8bd05eb5f244c59b in binutils.
    extern "C" int print_insn_i386 (bfd_vma, disassemble_info *);
    extern "C" int print_insn_big_arm(bfd_vma, disassemble_info *);
    extern "C" int print_insn_little_arm(bfd_vma, disassemble_info *);
#endif

#ifdef HAVE_CAPSTONE
#include <capstone.h>
#endif

using namespace Qt::Literals;

static int qstring_printf(void *data, const char *format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    auto size = std::vsnprintf(buffer, 1024, format, args);
    va_end(args);

    QString *s = static_cast<QString*>(data);
    s->append(QString::fromUtf8(buffer, size));
    return size;
}

#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 39)
static int qstring_printf_styled(void *data, [[maybe_unused]] enum disassembler_style style, const char* format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    auto size = std::vsnprintf(buffer, 1024, format, args);
    va_end(args);

    QString *s = static_cast<QString*>(data);
    s->append(QString::fromUtf8(buffer, size));
    return size;
}
#endif

static void print_address(bfd_vma addr, struct disassemble_info *info)
{
    const auto disasm = static_cast<Disassembler*>(info->application_data);
    assert(disasm);

    // TODO handle relocations/PLT/etc

    (*info->fprintf_func) (info->stream, "0x%lx", addr);
    auto s = static_cast<QString*>(info->stream);

    const uint64_t targetAddr = disasm->baseAddress() + addr;
    disasm->printAddress(targetAddr, s);
}

Disassembler::Disassembler() = default;

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
#if (defined(__x86_64__) || defined(__i386__)) && !defined Q_OS_FREEBSD
    if (file()->header()->machine() == EM_386 || file()->header()->machine() == EM_X86_64) {
        return disassembleBinutils(data, size);
    }
#endif

    return disassembleCapstone(data, size);
}

QString Disassembler::disassembleBinutils(const unsigned char* data, uint64_t size)
{
    QString result;
#ifndef Q_OS_FREEBSD

    disassembler_ftype disassemble_fn;
    disassemble_info info;
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 39)
    INIT_DISASSEMBLE_INFO(info, &result, qstring_printf, qstring_printf_styled);
#else
    INIT_DISASSEMBLE_INFO(info, &result, qstring_printf);
#endif

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
#if HAVE_DWARF
        auto line = lineForAddress(baseAddress() + bytes);
        if (!line.isNull())
            result += printSourceLine(line) + "<br/>"_L1;
#endif
        result += QStringLiteral("%1: ").arg(bytes, 8, 10);
        bytes += (*disassemble_fn)(bytes, &info);
        result += QLatin1String("<br/>");
    }
#endif

    return result;
}

#ifdef HAVE_CAPSTONE
static bool isInsnGroup(cs_insn *insn, uint8_t group)
{
    for (uint8_t i = 0; i < insn->detail->groups_count; ++i) {
        if (insn->detail->groups[i] == group)
            return true;
    }
    return false;
}
#endif

QString Disassembler::disassembleCapstone(const unsigned char* data, uint64_t size)
{
#ifdef HAVE_CAPSTONE
    csh handle;
    cs_err err;
    switch (file()->header()->machine()) {
        case EM_386:
            err = cs_open(CS_ARCH_X86, CS_MODE_32, &handle);
            break;
        case EM_X86_64:
            err = cs_open(CS_ARCH_X86, CS_MODE_64, &handle);
            break;
        case EM_ARM:
            err = cs_open(CS_ARCH_ARM, CS_MODE_LITTLE_ENDIAN, &handle);
            break;
        case EM_AARCH64:
            err = cs_open(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN, &handle);
            break;
        default:
            qWarning() << "Unsupported architecture!";
            return {};
    }
    if (err != CS_ERR_OK) {
        qWarning() << "Error opening Capstone handle:" << err;
        return {};
    }
    std::unique_ptr<csh, decltype(&cs_close)> handleGuard(&handle, &cs_close);
    cs_option(handle, CS_OPT_SYNTAX, CS_OPT_SYNTAX_ATT);
    cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);

    cs_insn *insn = cs_malloc(handle);
    const auto insnFree = [](cs_insn *insn) { cs_free(insn, 1); };
    std::unique_ptr<cs_insn, decltype(insnFree)> insnGuard(insn, insnFree);

    auto address = baseAddress();
    QString result;

    size_t cs_size = size; // force to size_t for 32bit host support
    while (cs_size > 0) {
        if (!cs_disasm_iter(handle, &data, &cs_size, &address, insn)) {
            return result;
        }

#if HAVE_DWARF
        const auto line = lineForAddress(insn->address);
        if (!line.isNull())
            result += printSourceLine(line) + "<br/>"_L1;
#endif

        result += QString::number(insn->address - baseAddress()) + ": "_L1 + QString::fromUtf8(insn->mnemonic) + QLatin1Char(' ') + QString::fromUtf8(insn->op_str);
        switch (file()->header()->machine()) {
            case EM_386:
            case EM_X86_64:
                for (int i = 0; i < insn->detail->x86.op_count; ++i) {
                    const auto op = insn->detail->x86.operands[i];
                    if (op.type == X86_OP_MEM) {
                        result += QLatin1String(" # 0x") + QString::number(op.mem.disp + address, 16);
                        printAddress(op.mem.disp + address, &result);
                    } else if (op.type == X86_OP_IMM) {
                        result += QLatin1String(" # 0x") + QString::number(op.imm, 16);
                        printAddress(op.imm, &result);
                    }
                }
                break;
            case EM_AARCH64:
                for (int i = 0; i < insn->detail->arm64.op_count; ++i) {
                    const auto op = insn->detail->arm64.operands[i];
                    if (op.type == ARM64_OP_MEM && (isInsnGroup(insn, CS_GRP_CALL) || isInsnGroup(insn, CS_GRP_JUMP))) {
                        result += QLatin1String(" # 0x") + QString::number(op.mem.disp + address, 16);
                        printAddress(op.mem.disp + address, &result);
                    } else if (op.type == ARM64_OP_IMM && (isInsnGroup(insn, CS_GRP_CALL) || isInsnGroup(insn, CS_GRP_JUMP) || insn->id == ARM64_INS_ADRP)) {
                        result += QLatin1String(" # 0x") + QString::number(op.imm, 16);
                        printAddress(op.imm, &result);
                    }
                }
                break;
            default:
                break;
        }
        result += "<br/>"_L1;
    }

    return result;
#else
    return {};
#endif
}

ElfFile* Disassembler::file() const
{
    return m_file;
}

uint64_t Disassembler::baseAddress() const
{
    return m_baseAddress;
}

void Disassembler::printAddress(uint64_t addr, QString *s) const
{
    if (auto symbolTable = file()->symbolTable()) {
        const auto target = symbolTable->entryContainingValue(addr);
        if (target) {
            s->append(" ("_L1);
            s->append(printSymbol(target));
            if (target->value() < addr) {
                s->append(QLatin1String("+0x") + QString::number(addr - target->value(), 16));
            }
            s->append(')'_L1);
            return;
        }
    }

    const auto secIdx = file()->indexOfSectionWithVirtualAddress(addr);
    if (secIdx < 0)
        return;

    const auto section = file()->section<ElfSection>(secIdx);
    assert(section);

    const auto pltSection = dynamic_cast<ElfPltSection*>(section);
    if (pltSection && section->header()->entrySize()) {
        const auto pltEntry = pltSection->entry((addr - section->header()->virtualAddress()) / section->header()->entrySize());
        assert(pltEntry);
        s->append(" ("_L1);
        s->append(printPltEntry(pltEntry));
        s->append(')'_L1);
        return;
    }

    const auto gotSection = dynamic_cast<ElfGotSection*>(section);
    if (gotSection) {
        const auto gotEntry = gotSection->entry((addr - section->header()->virtualAddress()) / file()->addressSize());
        assert(gotEntry);
        s->append(" ("_L1);
        s->append(printGotEntry(gotEntry));
        s->append(')'_L1);
        return;
    }

    s->append(QLatin1String(" (") + QString::fromUtf8(section->header()->name()) + QLatin1String(" + 0x") + QString::number(addr - section->header()->virtualAddress(), 16) + QLatin1Char(')'));
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
        return QString::fromUtf8(sym->name()) + QStringLiteral("@got");
    return QString::fromUtf8(entry->section()->header()->name()) + QStringLiteral(" + 0x") + QString::number(entry->index() * entry->section()->file()->addressSize());
}

QString Disassembler::printPltEntry(ElfPltEntry* entry) const
{
    const auto gotEntry = entry->gotEntry();
    const auto reloc = gotEntry ? gotEntry->relocation() : nullptr;
    const auto sym = reloc ? reloc->symbol() : nullptr;
    if (sym)
        return QString::fromUtf8(sym->name()) + QStringLiteral("@plt");
    return QString::fromUtf8(entry->section()->header()->name()) + QStringLiteral(" + 0x") + QString::number(entry->index() * entry->section()->header()->entrySize());
}

#if HAVE_DWARF
DwarfLine Disassembler::lineForAddress(uint64_t addr) const
{
    if (!file()->dwarfInfo())
        return {};

    auto cu = file()->dwarfInfo()->compilationUnitForAddress(addr);
    if (!cu)
        return {};
    return cu->lineForAddress(addr);
}

QString Disassembler::printSourceLine(DwarfLine line) const
{
    assert(!line.isNull());
    auto cu = file()->dwarfInfo()->compilationUnitForAddress(line.address());
    assert(cu);

    QUrl url;
    url.setScheme(QStringLiteral("code"));
    url.setPath(cu->sourceFileForLine(line));
    url.setFragment(QString::number(line.line()));

    QString s;
    s += "<i>Source: <a href=\""_L1 + QString::fromUtf8(url.toEncoded()) + "\">"_L1 + cu->sourceFileForLine(line);
    s += ':'_L1 + QString::number(line.line()) + "</a></i>"_L1;
    return s;
}
#endif
