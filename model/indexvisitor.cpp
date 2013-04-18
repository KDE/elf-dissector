#include "indexvisitor.h"

IndexVisitor::type IndexVisitor::doVisit(ElfFile* file, int row) const
{
    return qMakePair<void*, ElfNodeVariant::Type>(file->section<ElfSection>(row).get(), ElfNodeVariant::Section);
}

// IndexVisitor::type IndexVisitor::doVisit(ElfSymbolTableSection* symtab, int row) const
// {
//     return qMakePair<void*, ElfNodeVariant(symtab->entry(row), ElfNodeVariant::SymbolTableSection); // FIXME wrong type
// }
