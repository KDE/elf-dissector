#include "parentvisitor.h"
#include <elf/elffileset.h>

ParentVisitor::ParentVisitor(ElfFileSet* parent) : m_fileSet(parent)
{
}

QPair<void*, int> ParentVisitor::doVisit(ElfFile* file, int) const
{
    int row = 0;
    for (; row < m_fileSet->size(); ++row) {
        if (m_fileSet->file(row).get() == file)
            break;
    }
    return qMakePair<void*, int>(m_fileSet, row);
}

QPair<void*, int> ParentVisitor::doVisit(ElfSection* section, int) const
{
    return qMakePair<void*, int>(section->file(), section->header()->sectionIndex());
}
