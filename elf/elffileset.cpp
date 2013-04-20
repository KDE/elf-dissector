#include "elffileset.h"

ElfFileSet::ElfFileSet(QObject* parent) : QObject(parent)
{
}

ElfFileSet::~ElfFileSet()
{
    m_files.clear();
}

void ElfFileSet::addFile(const QString& fileName)
{
    ElfFile::Ptr f(new ElfFile(fileName));
    m_files.push_back(f);
}

int ElfFileSet::size() const
{
    return m_files.size();
}

ElfFile::Ptr ElfFileSet::file(int index) const
{
    return m_files.at(index);
}
