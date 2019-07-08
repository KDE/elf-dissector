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

#ifndef VIRTUALDTORCHECK_H
#define VIRTUALDTORCHECK_H

#include <QByteArray>
#include <QString>
#include <QVector>

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
    const QVector<Result>& results() const;
    void clear();

private:
    void findImplicitVirtualDtors(DwarfDie* die);

    QVector<Result> m_results;
};

#endif // VIRTUALDTORCHECK_H
