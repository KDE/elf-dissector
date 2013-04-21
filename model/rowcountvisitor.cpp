#include "rowcountvisitor.h"

#include <elf/elffileset.h>

int RowCountVisitor::doVisit(ElfFileSet* fileSet, int) const
{
    return fileSet->size();
}

int RowCountVisitor::doVisit(ElfFile* file, int) const
{
    return file->header()->sectionHeaderCount();
}

int RowCountVisitor::doVisit(ElfSymbolTableSection* symtab, int) const
{
    return symtab->header()->entryCount();
}

int RowCountVisitor::doVisit(ElfDynamicSection* section, int) const
{
    return section->header()->entryCount();
}
