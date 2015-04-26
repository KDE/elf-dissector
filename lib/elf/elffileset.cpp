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
#include "elfheader.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>

#include <cassert>

ElfFileSet::ElfFileSet(QObject* parent) : QObject(parent)
{
    parseLdConf();
    for (const auto &path : qgetenv("LD_LIBRARY_PATH").split(':'))
        m_baseSearchPaths.push_back(path);
}

ElfFileSet::~ElfFileSet()
{
    qDeleteAll(m_files);
}

void ElfFileSet::addFile(const QString& fileName)
{
    ElfFile* f = new ElfFile(fileName);
    if (!f->isValid()) {
        delete f;
        return;
    }
    addFile(f);
}

void ElfFileSet::addFile(ElfFile* file)
{
    assert(file);
    assert(file->isValid());

    m_files.push_back(file);

    if (!file->dynamicSection())
        return;

    const auto rpaths = file->dynamicSection()->rpaths();
    const auto runpaths = file->dynamicSection()->runpaths();
    QVector<QByteArray> searchPaths;
    searchPaths.reserve(rpaths.size() + m_ldLibraryPaths.size() + runpaths.size() + m_baseSearchPaths.size());
    searchPaths += rpaths;
    searchPaths += m_ldLibraryPaths;
    searchPaths += runpaths;
    searchPaths += m_baseSearchPaths;

    for (const auto &lib : file->dynamicSection()->neededLibraries()) {
        if (std::find_if(m_files.cbegin(), m_files.cend(), [lib](ElfFile *file){ return file->dynamicSection()->soName() == lib; }) != m_files.cend())
            continue;
        for (const auto &dir : searchPaths) {
            const auto fullPath = dir + '/' + lib;
            if (!QFile::exists(fullPath))
                continue;
            ElfFile *dep = new ElfFile(fullPath);
            if (dep->isValid() && dep->type() == m_files.first()->type() && dep->header()->machine() == m_files.first()->header()->machine()) {
                addFile(dep);
                break;
            }
            delete dep;
        }
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

        // we did not find one with unresolved dependencies, shouldn't happen, unless there's a cycle
        // so just take one and see how far we get
        if (sorted.at(i) == 0)
            sorted[i] = remaining.takeFirst();
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

void ElfFileSet::parseLdConf()
{
    parseLdConf("/etc/ld.so.conf");

    // built-in defaults
    m_baseSearchPaths.push_back("/lib64");
    m_baseSearchPaths.push_back("/lib");
    m_baseSearchPaths.push_back("/usr/lib64");
    m_baseSearchPaths.push_back("/usr/lib");
}

void ElfFileSet::parseLdConf(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << file.errorString();
        return;
    }

    while (!file.atEnd()) {
        const auto line = file.readLine().trimmed();
        if (line.startsWith('#'))
            continue;
        if (line.startsWith("include")) {
            const auto fileGlob = line.mid(8);
            if (QFileInfo::exists(fileGlob)) {
                parseLdConf(fileGlob);
            } else {
                const auto idx = fileGlob.lastIndexOf('/');
                assert(idx >= 0);
                QDir dir(fileGlob.left(idx));
                for (const auto &file : dir.entryList(QStringList() << fileGlob.mid(idx + 1)))
                    parseLdConf(dir.absolutePath() + '/' + file);
            }
            continue;
        }
        if (QFileInfo::exists(line)) {
            m_baseSearchPaths.push_back(line);
            continue;
        }
        qWarning() << "unable to handle ld.so.conf line:" << line;
    }
}
