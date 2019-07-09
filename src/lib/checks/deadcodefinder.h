/*
    Copyright (C) 2015 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef DEADCODEFINDER_H
#define DEADCODEFINDER_H

#include <QHash>
#include <QSet>
#include <QStringList>

class ElfFileSet;
class ElfFile;
class ElfSymbolTableEntry;


/** Identify unused exported symbols in a set of ELF objects. */
class DeadCodeFinder
{
public:
    DeadCodeFinder();
    DeadCodeFinder(const DeadCodeFinder&);
    ~DeadCodeFinder();

    DeadCodeFinder& operator=(const DeadCodeFinder&);

    void findUnusedSymbols(ElfFileSet *fileSet);
    void setExcludePrefixes(const QStringList &excludePrefixes);

    void dumpResults();

private:
    void scanUsage(ElfFile *file);

    void dumpResultsForFile(ElfFile *file);

    ElfFileSet *m_fileSet = nullptr;
    QHash<ElfFile*, QSet<ElfSymbolTableEntry*>> m_usedSymbols;
    QStringList m_excludePrefixes;
};

#endif // DEADCODEFINDER_H
