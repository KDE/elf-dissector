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

#include "elfdynamicsection.h"

#include <QByteArray>
#include <QVector>
#include <QList>

#include <elf.h>

ElfDynamicSection::ElfDynamicSection(ElfFile* file, ElfSectionHeader *shdr): ElfArraySection< ElfDynamicEntry >(file, shdr)
{
}

QByteArray ElfDynamicSection::soName() const
{
    for (const auto entry : m_entries) {
        if (entry->tag() == DT_SONAME) {
            const auto str = entry->stringValue();
            return QByteArray::fromRawData(str, strlen(str));
        }
    }
    return QByteArray();
}

QVector< QByteArray > ElfDynamicSection::neededLibraries() const
{
    return stringList(DT_NEEDED);
}

QVector< QByteArray > ElfDynamicSection::rpaths() const
{
    return stringList(DT_RPATH);
}

QVector< QByteArray > ElfDynamicSection::runpaths() const
{
    return stringList(DT_RUNPATH);
}

QVector< QByteArray > ElfDynamicSection::stringList(int64_t tag) const
{
    QVector<QByteArray> v;
    for (const auto entry : m_entries) {
        if (entry->tag() == tag) {
            const QByteArray s = entry->stringValue();
            foreach (const auto &b, s.split(':'))
                v.push_back(b);
        }
    }
    return v;
}

ElfDynamicEntry* ElfDynamicSection::entryWithTag(int64_t type) const
{
    for (const auto entry : m_entries) {
        if (entry->tag() == type)
            return entry;
    }
    return nullptr;
}
