/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
    QList<QByteArray> rpaths() const;
    /** RUNPATHs. */
    QList<QByteArray> runpaths() const;
    /** Needed libraries. */
    QList<QByteArray> neededLibraries() const;
    /** First entry of the given tag. @c nullptr if no such entry exists. */
    ElfDynamicEntry* entryWithTag(int64_t type) const;

private:
    QList<QByteArray> stringList(int64_t tag) const;
};

#endif // ELFDYNAMICSECTION_H
