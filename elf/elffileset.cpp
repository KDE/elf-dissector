/*
    Copyright (C) 2013-2014 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    qDeleteAll(m_files);
}

void ElfFileSet::addFile(const QString& fileName)
{
    ElfFile* f = new ElfFile(fileName);
    // TODO check if file is valid
    m_files.push_back(f);

    if (!f->dynamicSection())
        return;

    // TODO: this is likely not exactly correct
    for (const QByteArray &path : f->dynamicSection()->runpaths())
        m_searchPaths.push_front(QString::fromLocal8Bit(path));
    for (const QByteArray &path : f->dynamicSection()->rpaths())
        m_searchPaths.push_front(QString::fromLocal8Bit(path));

    for (const QByteArray &lib : f->dynamicSection()->neededLibraries()) {
        if (std::find_if(m_files.cbegin(), m_files.cend(), [lib](ElfFile *file){ return file->dynamicSection()->soName() == lib; }) != m_files.cend())
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

ElfFile* ElfFileSet::file(int index) const
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

static bool hasUnresolvedDependencies(ElfFile *file, const QVector<ElfFile*> &resolved, int startIndex)
{
    if (!file->dynamicSection())
        return false;

    for (const auto &lib : file->dynamicSection()->neededLibraries()) {
        const auto it = std::find_if(resolved.constBegin() + startIndex, resolved.constEnd(), [lib](ElfFile *file){ return file->dynamicSection()->soName() == lib; });
        if (it == resolved.constEnd()) {
            return true;
        }
    }
    return false;
}

void ElfFileSet::topologicalSort()
{
    QVector<ElfFile*> sorted;
    sorted.resize(m_files.size());

    QVector<ElfFile*> remaining = m_files;

    for (int i = sorted.size() - 1; i >= 0; --i) {
        for (auto it = std::begin(remaining); it != std::end(remaining); ++it) {
            if (!hasUnresolvedDependencies(*it, sorted, i + 1)) {
                sorted[i] = *it;
                remaining.erase(it);
                break;
            }
        }
        Q_ASSERT(sorted.at(i));
    }

#if 0
    qDebug() << "input";
    foreach(const auto f, m_files)
        qDebug() << f->displayName();
    qDebug() << "sorted";
    foreach(const auto f, sorted)
        qDebug() << f->displayName();
    qDebug() << "remaining";
    foreach(const auto f, remaining)
        qDebug() << f->displayName();
#endif

    Q_ASSERT(remaining.isEmpty());
    Q_ASSERT(sorted.first() == m_files.first());

    m_files = sorted;
}
