#include "elfdynamicsection.h"

#include <QByteArray>
#include <QVector>

#include <elf.h>

ElfDynamicSection::ElfDynamicSection(ElfFile* file, const ElfSectionHeader::Ptr& shdr): ElfArraySection< ElfDynamicEntry >(file, shdr)
{
}

QByteArray ElfDynamicSection::soName() const
{
    for (const ElfDynamicEntry::Ptr &entry : m_entries) {
        if (entry->tag() == DT_SONAME)
            return entry->stringValue();
    }
    return QByteArray();
}

QVector< QByteArray > ElfDynamicSection::neededLibraries() const
{
    return stringList(DT_NEEDED);
}

QVector< QByteArray > ElfDynamicSection::rpaths() const
{
    return stringList(DT_RPATH);
}

QVector< QByteArray > ElfDynamicSection::runpaths() const
{
    return stringList(DT_RUNPATH);
}

QVector< QByteArray > ElfDynamicSection::stringList(uint64_t tag) const
{
    QVector<QByteArray> v;
    for (const ElfDynamicEntry::Ptr &entry : m_entries) {
        if (entry->tag() == tag)
            v.push_back(entry->stringValue());
    }
    return v;
}
