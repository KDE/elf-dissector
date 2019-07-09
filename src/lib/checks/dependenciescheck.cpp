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

#include "dependenciescheck.h"

#include <elf/elffileset.h>
#include <elf/elffile.h>
#include <elf/elfsectionheader.h>
#include <elf/elfhashsection.h>
#include <elf/elfsymboltablesection.h>
#include <elf/elfsymboltableentry.h>

#include <QHash>

#include <cassert>
#include <iostream>

DependenciesCheck::UnusedDependencies DependenciesCheck::unusedDependencies(ElfFileSet* fileSet, int fileToCheck)
{
    QHash<QByteArray, int> fileIndex;
    for (int i = 0; i < fileSet->size(); ++i) {
        const auto file = fileSet->file(i);
        if (!file->dynamicSection())
            continue;
        const auto soName = file->dynamicSection()->soName();
        if (!soName.isEmpty())
            fileIndex.insert(soName, i);
        else
            fileIndex.insert(file->fileName().toUtf8(), i);
    }

    UnusedDependencies unusedDeps;
    for (int i = 0; i < fileSet->size(); ++i) {
        if (i != fileToCheck && fileToCheck >= 0)
            continue;
        const auto *dynamicSection = fileSet->file(i)->dynamicSection();
        if (!dynamicSection)
            continue;
        foreach (const auto &needed, fileSet->file(i)->dynamicSection()->neededLibraries()) {
            const auto depIdx = fileIndex.value(needed);
            const auto depFile = fileSet->file(depIdx);
            const auto count = usedSymbolCount(fileSet->file(i), depFile);
            if (count == 0)
                unusedDeps.push_back(qMakePair(i, depIdx));
        }
    }

    return unusedDeps;
}

void DependenciesCheck::printUnusedDependencies(ElfFileSet* fileSet, const UnusedDependencies& unusedDeps)
{
    for (auto unusedDep : unusedDeps) {
        std::cout << qPrintable(fileSet->file(unusedDep.first)->displayName()) << " depends on "
                  << qPrintable(fileSet->file(unusedDep.second)->displayName()) << " without using any of its symbols"
                  << std::endl;
    }
}

QVector<ElfSymbolTableEntry*> DependenciesCheck::usedSymbols(ElfFile* userFile, ElfFile* providerFile)
{
    QVector<ElfSymbolTableEntry*> symbols;

    const auto symtab = userFile->section<ElfSymbolTableSection>(userFile->indexOfSection(SHT_DYNSYM));
    if (!symtab)
        return symbols;
    const auto symtabSize = symtab->header()->entryCount();

    const auto hashtab = providerFile->hash();
    assert(hashtab);

    for (uint i = 0; i < symtabSize; ++i) {
        const auto userEntry = symtab->entry(i);
        if (userEntry->value() != 0)
            continue;
        const auto providerEntry = hashtab->lookup(userEntry->name());
        if (providerEntry && providerEntry->value() > 0)
            symbols.push_back(providerEntry);
    }

    return symbols;
}

int DependenciesCheck::usedSymbolCount(ElfFile* userFile, ElfFile* providerFile)
{
    const auto symtab = userFile->section<ElfSymbolTableSection>(userFile->indexOfSection(SHT_DYNSYM));
    if (!symtab)
        return 0;
    const auto symtabSize = symtab->header()->entryCount();

    const auto hashtab = providerFile->hash();
    assert(hashtab);

    int count = 0;
    for (uint i = 0; i < symtabSize; ++i) {
        const auto userEntry = symtab->entry(i);
        if (userEntry->value() != 0)
            continue;
        const auto providerEntry = hashtab->lookup(userEntry->name());
        if (providerEntry && providerEntry->value() > 0)
            ++count;
    }
    return count;
}
