#include "rowcountvisitor.h"

int RowCountVisitor::doVisit(ElfFile* file, int) const
{
    return file->header()->sectionHeaderCount();
}

int RowCountVisitor::doVisit(ElfSymbolTableSection* symtab, int) const
{
    return 0;
}
