#include "elfsymboltableentry.h"
#include "elfsymboltablesection.h"
#include "elfstringtablesection.h"

#include <QByteArray>

#include <elf.h>

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

uint8_t ElfSymbolTableEntry::bindType() const
{
    // same as 64
    return ELF32_ST_BIND(info());
}

uint8_t ElfSymbolTableEntry::type() const
{
    // same as 64
    return ELF32_ST_TYPE(info());
}
