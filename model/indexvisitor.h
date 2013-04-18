#ifndef INDEXVISITOR_H
#define INDEXVISITOR_H

#include "elfnodevisitor.h"

#include <QPair>

class IndexVisitor : public ElfNodeVisitor<QPair<void*, ElfNodeVariant::Type>>
{
public:
    type doVisit(ElfFile* file, int row) const override;
//     type doVisit(ElfSymbolTableSection* symtab, int row) const override;
};

#endif // INDEXVISITOR_H
