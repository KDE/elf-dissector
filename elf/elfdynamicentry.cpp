#include "elfdynamicentry.h"
#include "elfdynamicsection.h"
#include "elfstringtablesection.h"
#include <QObject>

#include <elf.h>

#include <cassert>

ElfDynamicEntry::ElfDynamicEntry(const ElfDynamicSection* section) : m_section(section)
{
}

ElfDynamicEntry::~ElfDynamicEntry()
{
}

const ElfDynamicSection* ElfDynamicEntry::dynamicSection() const
{
    return m_section;
}

QString ElfDynamicEntry::tagName() const
{
    switch (tag()) {
        case DT_NULL: return "<null>";
        case DT_NEEDED: return "Needed library";
        case DT_PLTRELSZ: return "PLT reloc size";
        case DT_PLTGOT: return "PLTGOT";
        case DT_HASH: return "Symbol hash table address";
        case DT_STRTAB: return "String table address";
        case DT_SYMTAB: return "Symbol table address";
        case DT_RELA: return "Rela reloc address";
        case DT_RELASZ: return "Rela reloc size";
        case DT_RELAENT: return "Rela reloc entry size";
        case DT_STRSZ: return "String table size";
        case DT_SYMENT: return "Symbol table entry size";
        case DT_INIT: return "Init function address";
        case DT_FINI: return "Termination function address";
        case DT_SONAME: return "Shared object name";
        case DT_RPATH: return "RPATH";
        case DT_SYMBOLIC: return "Symbol search start";
        case DT_REL: return "Rel reloc address";
        case DT_RELSZ: return "Rel reloc size";
        case DT_RELENT: return "Rel reloc entry size";
        case DT_PLTREL: return "Reloc type in PLT";
        case DT_DEBUG: return "Debug";
        case DT_TEXTREL: return "Reloc might modify .text";
        case DT_JMPREL: return "PLT reloc address";
        case DT_BIND_NOW: return "BIND_NOW";
        case DT_INIT_ARRAY: return "Init function address array";
        case DT_FINI_ARRAY: return "Termination function address array";
        case DT_INIT_ARRAYSZ: return "Init function address array size";
        case DT_FINI_ARRAYSZ: return "Termination function address array size";
        case DT_RUNPATH: return "RUNPATH";
        case DT_FLAGS: return "Flags";
        case DT_PREINIT_ARRAY: return "Preinit function address array";
        case DT_PREINIT_ARRAYSZ: return "Preinit function address array size";
        case DT_SYMINSZ: return "Syminfo table size";
        case DT_SYMINENT: return "Syminfo table entry size";
        case DT_GNU_HASH: return "GNU hash table address";
        case DT_VERSYM: return "GNU version symbol";
        case DT_VERNEED: return "Needed versions table address";
        case DT_VERNEEDNUM: return "Number of needed versions";
        default:
            return QObject::tr("unknown (%1)").arg(tag());
    }

    return QString();
}

bool ElfDynamicEntry::isStringValue() const
{
    return tag() == DT_NEEDED
        || tag() == DT_SONAME
        || tag() == DT_RPATH
        || tag() == DT_RUNPATH
    ; // TODO complete
}

const char* ElfDynamicEntry::stringValue() const
{
    assert(isStringValue());
    return dynamicSection()->linkedSection<ElfStringTableSection>()->string(value());
}
