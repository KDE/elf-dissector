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

#include "ldbenchmark.h"

#include <elf/elffile.h>
#include <elf/elffileset.h>

#include <QDebug>
#include <QProcess>
#include <QStringList>

#include <algorithm>
#include <iostream>

#include <cassert>

LDBenchmark::Result::Result() :
    lazyTotal(0.0),
    nowTotal(0.0),
    lazyCount(0),
    nowCount(0)
{
}

double LDBenchmark::Result::averageLazy() const
{
    if (lazyCount == 0)
        return 0.0;
    return lazyTotal / lazyCount;
}

double LDBenchmark::Result::averageNow() const
{
    if (nowCount == 0)
        return 0.0;
    return nowTotal / nowCount;
}


void LDBenchmark::measureFileSet(ElfFileSet* fileSet)
{
    m_fileSet = fileSet;

    m_results.clear();
    m_results.reserve(fileSet->size());

    QStringList args;
    args.reserve(fileSet->size() + 1);
    args.push_back("RTLD_LAZY");

    for (int i = fileSet->size() - 1; i >= 0; --i) {
        const auto fileName = fileSet->file(i)->displayName();
        args.push_back(fileName);
        Result r;
        r.fileName = fileName.toUtf8();
        m_results.push_back(r);
    }

    for (int i = 0; i < 5; ++i) {
        QProcess proc;
        proc.setProcessChannelMode(QProcess::QProcess::ForwardedErrorChannel);
        proc.start("ldbenchmark-runner", args); // TODO find in libexec
        proc.waitForFinished();
        readResults(&proc, LoadMode::Lazy);
    }

    args[0] = "RTLD_NOW";
    for (int i = 0; i < 5; ++i) {
        QProcess proc;
        proc.setProcessChannelMode(QProcess::QProcess::ForwardedErrorChannel);
        proc.start("ldbenchmark-runner", args); // TODO find in libexec
        proc.waitForFinished();
        readResults(&proc, LoadMode::Now);
    }

    dumpResults();
}

void LDBenchmark::readResults(QProcess* proc, LoadMode mode)
{
    while(proc->canReadLine()) {
        const auto line = proc->readLine();
        const auto index = line.lastIndexOf('\t');
        const auto fileName = line.left(index);
        const auto cost = line.mid(index).trimmed().toDouble();
        auto it = std::find_if(m_results.begin(), m_results.end(), [fileName](const Result &res) {
            return res.fileName == fileName;
        });
        assert(it != m_results.end());

        switch (mode) {
            case LoadMode::Lazy:
                (*it).lazyTotal += cost;
                (*it).lazyCount++;
                break;
            case LoadMode::Now:
                (*it).nowTotal += cost;
                (*it).nowCount++;
                break;
        }
    }
}

void LDBenchmark::writeCSV(const QString& fileName)
{
    QFile f(fileName);
    if (!f.open(QFile::WriteOnly | QFile::Truncate)) {
        qWarning() << "Failed to open" << fileName;
        return;
    }

    for (int i = 0; i < m_results.size(); ++i) {
        const auto res = m_results.at(i);
        f.write(res.fileName.constData());
        f.write("\t");
        f.write(QByteArray::number(res.averageLazy()));
        f.write("\t");
        f.write(QByteArray::number(res.averageNow()));
        f.write("\n");
    }
}

void LDBenchmark::dumpResults()
{
    double lazy = 0.0;
    double now = 0.0;
    for (int i = 0; i < m_results.size(); ++i) {
        const auto res = m_results.at(i);
        const auto file = m_fileSet->file(m_results.size() - 1 - i);
        printf("%s\t%.2f\t%.2f\n", res.fileName.constData(), res.averageLazy(), res.averageNow());
        lazy += res.averageLazy();
        now += res.averageNow();
    }
    printf("Lazy: %.2f µs, immediate: %.2f µs\n", lazy, now);
}
