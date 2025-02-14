/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DEPENDENCYSORTER_H
#define DEPENDENCYSORTER_H

class ElfFileSet;

/** Sorts DT_NEEDED entries of .dynamic by symbol lookup hit probability. */
class DependencySorter
{
public:
    void sortDtNeeded(ElfFileSet* fileSet);
};

#endif // DEPENDENCYSORTER_H
