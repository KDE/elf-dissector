#include "elffileset.h"

#include <QDebug>

ElfFileSet::ElfFileSet(QObject* parent) : QObject(parent)
{
    m_searchPaths.push_back("/lib64");
    m_searchPaths.push_back("/usr/lib64"); // TODO
    for (const QByteArray &path : qgetenv("LD_LIBRARY_PATH").split(':'))
        m_searchPaths.push_back(QString::fromLocal8Bit(path));
}

ElfFileSet::~ElfFileSet()
{
    m_files.clear();
}

void ElfFileSet::addFile(const QString& fileName)
{
    ElfFile::Ptr f(new ElfFile(fileName));
    // TODO check if file is valid
    m_files.push_back(f);

    if (!f->dynamicSection())
        return;

    // TODO: this is likely not exactly correct
    for (const QByteArray &path : f->dynamicSection()->rpaths())
        m_searchPaths.push_back(QString::fromLocal8Bit(path));
    for (const QByteArray &path : f->dynamicSection()->runpaths())
        m_searchPaths.push_back(QString::fromLocal8Bit(path));

    for (const QByteArray &lib : f->dynamicSection()->neededLibraries()) {
        if (std::find_if(m_files.cbegin(), m_files.cend(), [lib](const ElfFile::Ptr &file){ return file->dynamicSection()->soName() == lib; }) != m_files.cend())
            continue;
        const QString &libFile = findLibrary(QString::fromLocal8Bit(lib));
        qDebug() << Q_FUNC_INFO << libFile << lib;
        if (!libFile.isEmpty())
            addFile(libFile);
    }
}

int ElfFileSet::size() const
{
    return m_files.size();
}

ElfFile::Ptr ElfFileSet::file(int index) const
{
    return m_files.at(index);
}

QString ElfFileSet::findLibrary(const QString& name) const
{
    for (const QString &dir : m_searchPaths) {
        const QString libPath = dir + "/" + name;
        if (QFile::exists(libPath))
            return libPath;
    }

    return QString();
}
