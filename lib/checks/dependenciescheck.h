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

#ifndef DEPENDENCIESCHECK_H
#define DEPENDENCIESCHECK_H

class ElfFileSet;
class ElfFile;
class ElfSymbolTableEntry;

#include <QPair>
#include <QVector>

namespace DependenciesCheck
{
    using UnusedDependencies = QVector<QPair<int, int>>;
    /** Find all unused DT_NEEDED entries in the entire file set. */
    UnusedDependencies unusedDependencies(ElfFileSet *fileSet, int fileToCheck = -1);

    /** Dump unused dependencies to stdout, for use in CLI tools. */
    void printUnusedDependencies(ElfFileSet *fileSet, const UnusedDependencies &unusedDeps);

    /** Returns a list of symbols of @p providerFile used by @p userFile. */
    QVector<ElfSymbolTableEntry*> usedSymbols(ElfFile *userFile, ElfFile* providerFile);
    /** Returns the amount of symbols from @p providerFile used by @p userFile. */
    int usedSymbolCount(ElfFile *userFile, ElfFile* providerFile);
}

#endif // DEPENDENCIESCHECK_H
