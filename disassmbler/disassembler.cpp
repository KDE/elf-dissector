#include "disassembler.h"

#include <elf/elfsymboltableentry.h>
#include <elf/elfsymboltablesection.h>
#include <elf/elffile.h>

#include <QString>

#include <cstdarg>
#include <dis-asm.h>
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

QString Disassembler::disassemble(ElfSymbolTableEntry* entry) const
{
    QString result;
    disassembler_ftype disassemble_fn;
    disassemble_info info;
    INIT_DISASSEMBLE_INFO(info, &result, qstring_printf);

    // TODO: read correctly from entry->section->file->header
    info.flavour = bfd_target_elf_flavour;
    info.arch = bfd_arch_i386;
    info.mach = bfd_mach_x86_64;
    info.endian = BFD_ENDIAN_LITTLE;
    disassemble_fn = print_insn_i386;

    info.buffer = const_cast<bfd_byte*>(entry->symbolTable()->file()->rawData());
    info.buffer += entry->symbolTable()->file()->sectionHeaders().at(entry->sectionIndex())->sectionOffset();
    info.buffer += entry->value();
    info.buffer_length = entry->size();
    info.buffer_vma = 0;

    uint32_t bytes = 0;
    while (bytes < entry->size()) {
        result += QString::fromLatin1("%1: ").arg(bytes, 8, 10);
        bytes += (*disassemble_fn)(bytes, &info);
        result += "<br/>";
    }

    return result;
}