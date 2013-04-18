#ifndef PARENTVISITOR_H
#define PARENTVISITOR_H

#include "elfnodevisitor.h"

#include <QPair>

class ParentVisitor : public ElfNodeVisitor<QPair<void*, int>>
{
protected:
    type doVisit(ElfFile* file, int) const override;
    type doVisit(ElfSection* section, int) const override;
};

#endif // PARENTVISITOR_H
