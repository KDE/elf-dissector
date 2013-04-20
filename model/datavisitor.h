#ifndef DATAVISITOR_H
#define DATAVISITOR_H

#include "elfnodevisitor.h"

#include <QVariant>

class DataVisitor : public ElfNodeVisitor<QVariant>
{
protected:
    QVariant doVisit(ElfFile* file, int arg) const override;
    QVariant doVisit(ElfSection* section, int arg) const override;
};

#endif // DATAVISITOR_H
