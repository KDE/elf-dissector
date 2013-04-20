#ifndef PARENTVISITOR_H
#define PARENTVISITOR_H

#include "elfnodevisitor.h"

#include <QPair>

class ParentVisitor : public ElfNodeVisitor<QPair<void*, int>>
{
public:
    explicit ParentVisitor(ElfFileSet* parent);

protected:
    type doVisit(ElfFile* file, int) const override;
    type doVisit(ElfSection* section, int) const override;

private:
    ElfFileSet *m_fileSet;
};

#endif // PARENTVISITOR_H
