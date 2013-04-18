#ifndef ELFNODEVISITOR_H
#define ELFNODEVISITOR_H

#include <elf/elffile.h>
#include <elf/elfheader.h>
#include <elf/elfsymboltablesection.h>

#include "elfnodevariant.h"

#include <cassert>

class ElfSymbolTableSection;
class ElfFile;

template <typename T>
class ElfNodeVisitor
{
public:
    typedef T type;

    inline T visit(ElfNodeVariant *node, int arg = 0) const
    {
        switch (node->type) {
            case ElfNodeVariant::Invalid:
                assert(false);
                break;
            case ElfNodeVariant::File:
                return doVisit(node->value<ElfFile>(), arg);
            case ElfNodeVariant::Section:
                return doVisit(node->value<ElfSection>(), arg);
            case ElfNodeVariant::SymbolTableSection:
                return doVisit(node->value<ElfSymbolTableSection>(), arg);
        }

        assert(false);
    }

protected:
    virtual T doVisit(ElfFile* file, int arg) const
    {
        Q_UNUSED(file);
        Q_UNUSED(arg);
        return T();
    }
    virtual T doVisit(ElfSection* section, int arg) const
    {
        Q_UNUSED(section);
        Q_UNUSED(arg);
        return T();
    }
    virtual T doVisit(ElfSymbolTableSection *symtab, int arg) const
    {
        return doVisit(static_cast<ElfSection*>(symtab), arg);
    }
};

#endif
