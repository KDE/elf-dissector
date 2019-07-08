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
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef ELFDYNAMICSECTION_H
#define ELFDYNAMICSECTION_H

#include "elfarraysection.h"
#include "elfdynamicentry.h"

class ElfDynamicSection : public ElfArraySection<ElfDynamicEntry>
{
public:
    explicit ElfDynamicSection(ElfFile* file, ElfSectionHeader *shdr);

    /** SO name. */
    QByteArray soName() const;
    /** RPATHs. */
    QVector<QByteArray> rpaths() const;
    /** RUNPATHs. */
    QVector<QByteArray> runpaths() const;
    /** Needed libraries. */
    QVector<QByteArray> neededLibraries() const;
    /** First entry of the given tag. @c nullptr if no such entry exists. */
    ElfDynamicEntry* entryWithTag(int64_t type) const;

private:
    QVector<QByteArray> stringList(int64_t tag) const;
};

#endif // ELFDYNAMICSECTION_H
