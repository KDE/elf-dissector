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

#include "dependencysorter.h"
#include <checks/dependenciescheck.h>
#include <elf/elffileset.h>

#include <QDebug>
#include <QHash>

#include <cassert>
#include <elf.h>

void DependencySorter::sortDtNeeded(ElfFileSet* fileSet)
{
    assert(fileSet->size() > 0);

    auto file = fileSet->file(0);
    if (!file->dynamicSection())
        return;

    // TODO index SO_NAME, this probably should be moved to ElfFileSet, we have that in a bunch of places now
    QHash<QByteArray, int> nameIndex;
    for (int i = 0; i < fileSet->size(); ++i) {
        const auto f = fileSet->file(i);
        if (!f->dynamicSection())
            continue;
        const auto soName = f->dynamicSection()->soName();
        if (nameIndex.contains(soName)) {
            qWarning() << "Suspicious DT_NEEDED entry '" << soName << "' in " << f->fileName() << ", aborting.";
            return;
        }

        if (!soName.isEmpty())
            nameIndex.insert(soName, i);
    }

    // count usages
    QVector<int> usageCounts;
    const auto needed = file->dynamicSection()->neededLibraries();
    usageCounts.resize(needed.size());
    for (int i = 0; i < needed.size(); ++i) {
        auto depFile = fileSet->file(nameIndex.value(needed.at(i)));
        assert(depFile);
        assert(file != depFile);

        usageCounts[i] = DependenciesCheck::usedSymbolCount(file, depFile);
    }
    qDebug() << usageCounts;

    // sort, check if change is needed
    QVector<int> sortedNeededIndex;
    sortedNeededIndex.resize(needed.size());
    std::iota(sortedNeededIndex.begin(), sortedNeededIndex.end(), 0);
    assert(sortedNeededIndex.size() == usageCounts.size());
    qDebug() << sortedNeededIndex;

    std::sort(sortedNeededIndex.begin(), sortedNeededIndex.end(), [usageCounts](int lhs, int rhs) {
        return usageCounts.at(lhs) > usageCounts.at(rhs);
    });

    qDebug() << sortedNeededIndex;

    // since we modify the file in-place, get the necessary string table values before we do that
    QVector<uint64_t> neededValues;
    neededValues.resize(needed.size());
    int neededIndex = 0;
    for (uint i = 0; i < file->dynamicSection()->header()->entryCount(); ++i) {
        auto dynEntry = file->dynamicSection()->entry(i);
        if (dynEntry->tag() != DT_NEEDED)
            continue;
        neededValues[neededIndex++] = dynEntry->value();
    }

    // open target file
    ElfFile newFile(file->fileName());
    if (!newFile.open(QFile::ReadWrite) || !newFile.isValid()) {
        qWarning() << "Can't open" << file->fileName() << "for writing.";
        return;
    }

    // write change
    neededIndex = 0;
    auto newDynSection = newFile.dynamicSection();
    for (uint i = 0; i < newDynSection->header()->entryCount(); ++i) {
        auto dynEntry = newDynSection->entry(i);
        if (dynEntry->tag() != DT_NEEDED)
            continue;

        dynEntry->setValue(neededValues.at(sortedNeededIndex.at(neededIndex)));

        qDebug() << needed.at(sortedNeededIndex.at(neededIndex));
        ++neededIndex;
    }

}
