#ifndef ROWCOUNTVISITOR_H
#define ROWCOUNTVISITOR_H

#include "elfnodevisitor.h"

// TODO rename to "ChildCountVisitor"
class RowCountVisitor : public ElfNodeVisitor<int>
{
protected:
    int doVisit(ElfFileSet* fileSet, int arg) const override;
    int doVisit(ElfFile* file, int) const override;
    int doVisit(ElfSymbolTableSection* symtab, int) const override;
};



#endif