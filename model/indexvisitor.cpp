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
        case SHT_DYNSYM:
            type = ElfNodeVariant::SymbolTableSection;
            break;
        case SHT_DYNAMIC:
            type = ElfNodeVariant::DynamicSection;
            break;
        default:
            type = ElfNodeVariant::Section;
    }
    return qMakePair<void*, ElfNodeVariant::Type>(section, type);
}

IndexVisitor::type IndexVisitor::doVisit(ElfSymbolTableSection* symtab, int row) const
{
    const ElfSymbolTableEntry::Ptr entry = symtab->entry(row);
    return qMakePair<void*, ElfNodeVariant::Type>(entry.get(), ElfNodeVariant::SymbolTableEntry);
}

IndexVisitor::type IndexVisitor::doVisit(ElfDynamicSection* section, int row) const
{
    const ElfDynamicEntry::Ptr entry = section->entry(row);
    return qMakePair<void*, ElfNodeVariant::Type>(entry.get(), ElfNodeVariant::DynamicEntry);
}
