/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "elfdynamicsection.h"

#include <QByteArray>
#include <QList>
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

QList< QByteArray > ElfDynamicSection::neededLibraries() const
{
    return stringList(DT_NEEDED);
}

QList< QByteArray > ElfDynamicSection::rpaths() const
{
    return stringList(DT_RPATH);
}

QList< QByteArray > ElfDynamicSection::runpaths() const
{
    return stringList(DT_RUNPATH);
}

QList< QByteArray > ElfDynamicSection::stringList(int64_t tag) const
{
    QList<QByteArray> v;
    for (const auto entry : m_entries) {
        if (entry->tag() == tag) {
            const QByteArray s = entry->stringValue();
            for (const auto &b : s.split(':'))
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
