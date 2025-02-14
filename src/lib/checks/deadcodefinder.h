/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
