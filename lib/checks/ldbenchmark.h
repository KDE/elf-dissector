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

#ifndef LDBENCHMARK_H
#define LDBENCHMARK_H

#include <QByteArray>
#include <QStringList>
#include <QVector>

class QProcess;

class ElfFileSet;

/** Load all libraries in dependency order one by one and measure the needed time. */
class LDBenchmark
{
public:
    void measureFileSet(ElfFileSet *fileSet);

    void writeCSV(const QString &fileName);

private:
    enum class LoadMode { None, Now, Lazy };

    void measure(LoadMode mode, int iterations);
    void readResults(QProcess *proc, LoadMode mode);
    void dumpResults();

    ElfFileSet *m_fileSet = nullptr;

    struct Result {
        Result();
        QByteArray fileName;
        double lazyTotal;
        double nowTotal;
        int lazyCount;
        int nowCount;

        double averageLazy() const;
        double averageNow() const;
    };
    QVector<Result> m_results;
    QStringList m_args;
};

#endif // LDBENCHMARK_H
