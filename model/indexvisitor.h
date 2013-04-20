#ifndef INDEXVISITOR_H
#define INDEXVISITOR_H

#include "elfnodevisitor.h"

#include <QPair>

// TODO: better name "ChildVisitor"?
class IndexVisitor : public ElfNodeVisitor<QPair<void*, ElfNodeVariant::Type>>
{
public:
    type doVisit(ElfFileSet* fileSet, int row) const override;
    type doVisit(ElfFile* file, int row) const override;
//     type doVisit(ElfSymbolTableSection* symtab, int row) const override;
};

#endif // INDEXVISITOR_H
