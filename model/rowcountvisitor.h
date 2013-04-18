#ifndef ROWCOUNTVISITOR_H
#define ROWCOUNTVISITOR_H

#include "elfnodevisitor.h"

class RowCountVisitor : public ElfNodeVisitor<int>
{
public:
    using ElfNodeVisitor::visit;
protected:
    int doVisit(ElfFile* file, int) const override;
    int doVisit(ElfSymbolTableSection* symtab, int) const override;
};



#endif