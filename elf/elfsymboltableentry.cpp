#include "elfsymboltableentry.h"
#include "elfsymboltablesection.h"
#include "elfstringtablesection.h"

#include <QByteArray>

ElfSymbolTableEntry::ElfSymbolTableEntry(const ElfSymbolTableSection* section) :  m_section(section)
{
}

ElfSymbolTableEntry::~ElfSymbolTableEntry()
{
}

const char* ElfSymbolTableEntry::name() const
{
    return m_section->linkedSection<ElfStringTableSection>()->string(nameIndex());
}

QByteArray ElfSymbolTableEntry::prettyName() const
{
    return QByteArray(); // TODO
}

