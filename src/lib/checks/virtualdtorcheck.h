/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef VIRTUALDTORCHECK_H
#define VIRTUALDTORCHECK_H

#include <QByteArray>
#include <QString>
#include <QList>

class ElfFileSet;
class DwarfDie;

/** Find implicit virtual dtors. */
class VirtualDtorCheck
{
public:
    void findImplicitVirtualDtors(ElfFileSet* fileSet);
    void printResults() const;

    struct Result {
        QByteArray fullName;
        QString sourceFilePath;
        int lineNumber;
    };
    const QList<Result>& results() const;
    void clear();

private:
    void findImplicitVirtualDtors(DwarfDie* die);

    QList<Result> m_results;
};

#endif // VIRTUALDTORCHECK_H
