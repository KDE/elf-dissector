#include "indexvisitor.h"

#include <elf/elffile.h>
#include <elf/elffileset.h>

#include <elf.h>

IndexVisitor::type IndexVisitor::doVisit(ElfFileSet* fileSet, int row) const
{
    return qMakePair<void*, ElfNodeVariant::Type>(fileSet->file(row).get(), ElfNodeVariant::File);
}

IndexVisitor::type IndexVisitor::doVisit(ElfFile* file, int row) const
{
    ElfSection *section = file->section<ElfSection>(row).get();
    ElfNodeVariant::Type type;
    switch (section->header()->type()) {
        case SHT_SYMTAB:
            type = ElfNodeVariant::SymbolTableSection;
            break;
        default:
            type = ElfNodeVariant::Section;
    }
    return qMakePair<void*, ElfNodeVariant::Type>(section, type);
}

IndexVisitor::type IndexVisitor::doVisit(ElfSymbolTableSection* symtab, int row) const
{
    // TODO memory management!
    ElfSymbolTableSection::ElfSymbolTableEntry *entry = symtab->entry(row);
    return qMakePair<void*, ElfNodeVariant::Type>(entry, ElfNodeVariant::SymbolTableEntry);
}
