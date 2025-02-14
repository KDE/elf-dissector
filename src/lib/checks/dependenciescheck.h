/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DEPENDENCIESCHECK_H
#define DEPENDENCIESCHECK_H

class ElfFileSet;
class ElfFile;
class ElfSymbolTableEntry;

#include <QPair>
#include <QList>

namespace DependenciesCheck
{
    using UnusedDependencies = QList<QPair<int, int>>;
    /** Find all unused DT_NEEDED entries in the entire file set. */
    UnusedDependencies unusedDependencies(ElfFileSet *fileSet, int fileToCheck = -1);

    /** Dump unused dependencies to stdout, for use in CLI tools. */
    void printUnusedDependencies(ElfFileSet *fileSet, const UnusedDependencies &unusedDeps);

    /** Returns a list of symbols of @p providerFile used by @p userFile. */
    QList<ElfSymbolTableEntry*> usedSymbols(ElfFile *userFile, ElfFile* providerFile);
    /** Returns the amount of symbols from @p providerFile used by @p userFile. */
    int usedSymbolCount(ElfFile *userFile, ElfFile* providerFile);
}

#endif // DEPENDENCIESCHECK_H
