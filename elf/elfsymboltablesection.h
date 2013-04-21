#ifndef ELFSYMBOLTABLESECTION_H
#define ELFSYMBOLTABLESECTION_H

#include "elfsection.h"
#include "elfsymboltableentry.h"

#include <QVector>

/** Size-independent adaptor for symbol table sections. */
class ElfSymbolTableSection : public ElfSection
{
public:
    virtual ~ElfSymbolTableSection();
    ElfSymbolTableEntry::Ptr entry(uint32_t index) const;

protected:
    explicit ElfSymbolTableSection(ElfFile *file, const ElfSectionHeader::Ptr &shdr);
    virtual ElfSymbolTableEntry::Ptr createEntry(uint32_t index) const = 0;
    /** Must be called from sub-class ctor. */
    void parse();

private:
    QVector<ElfSymbolTableEntry::Ptr> m_entries;
};

#endif // ELFSYMBOLTABLESECTION_H
