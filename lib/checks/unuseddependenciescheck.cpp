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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "unuseddependenciescheck.h"

#include <elf/elffileset.h>
#include <elf/elffile.h>
#include <elf/elfsectionheader.h>
#include <elf/elfhashsection.h>
#include <elf/elfsymboltablesection.h>
#include <elf/elfsymboltableentry.h>

#include <QHash>

#include <iostream>

UnusedDependenciesCheck::UnusedDependenciesCheck()
{
}

UnusedDependenciesCheck::~UnusedDependenciesCheck() = default;

void UnusedDependenciesCheck::checkFileSet(ElfFileSet* fileSet)
{
    QHash<QByteArray, int> fileIndex;
    for (int i = 0; i < fileSet->size(); ++i) {
        const auto file = fileSet->file(i);
        if (!file->dynamicSection())
            continue;
        const auto soName = file->dynamicSection()->soName();
        if (!soName.isEmpty())
            fileIndex.insert(soName, i);
    }

    for (int i = 0; i < fileSet->size(); ++i) {
        const auto userSymbolTable = fileSet->file(i)->symbolTable();
        if (!userSymbolTable)
            continue;
        const auto userSymbolTableSize = userSymbolTable->header()->entryCount();

        for (const auto &needed : fileSet->file(i)->dynamicSection()->neededLibraries()) {
            const auto depFile = fileSet->file(fileIndex.value(needed));
            const auto depHash = depFile->hash();
            int count = 0;
            for (uint k = 0; k < userSymbolTable->header()->entryCount(); ++k) {
                const auto entry = userSymbolTable->entry(k);
                if (entry->value() != 0)
                    continue;
                if (depHash->lookup(entry->name()))
                    ++count;
            }

            if (count == 0)
                std::cout << qPrintable(fileSet->file(i)->displayName()) << " depends on " << needed.constData() << " without using any of its symbols" << std::endl;
        }
    }
}
