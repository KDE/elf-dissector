#include "parentvisitor.h"

QPair<void*, int> ParentVisitor::doVisit(ElfFile* file, int) const
{
    return qMakePair<void*, int>(nullptr, 0);
}

QPair<void*, int> ParentVisitor::doVisit(ElfSection* section, int) const
{
    return qMakePair<void*, int>(section->file(), section->header()->sectionIndex());
}
