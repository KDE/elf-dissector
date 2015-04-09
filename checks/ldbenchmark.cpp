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

LDBenchmark::Result::Result() :
    lazyTotal(0.0),
    nowTotal(0.0),
    lazyCount(0),
    nowCount(0)
{
}

void LDBenchmark::measureFileSet(ElfFileSet* fileSet)
{
    QStringList args;
    args.reserve(fileSet->size() + 1);
    args.push_back("RTLD_LAZY");
    for (int i = fileSet->size() - 1; i >= 0; --i) {
        args.push_back(fileSet->file(i)->displayName());
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
        if (it == m_results.end()) {
            Result r;
            r.fileName = fileName;
            m_results.push_back(r);
            it = m_results.end() - 1;
        }

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

void LDBenchmark::dumpResults()
{
    std::sort(m_results.begin(), m_results.end(), [](const Result &lhs, const Result &rhs) {
        return lhs.lazyTotal > rhs.lazyTotal;
    });

    double lazy = 0.0;
    double now = 0.0;
    for (const auto &res : m_results) {
        const auto avgLazy = res.lazyTotal / res.lazyCount;
        const auto avgNow = res.nowTotal / res.nowCount;
        printf("%s\t%.2f\t%.2f\n", res.fileName.constData(), avgLazy, avgNow);
        lazy += avgLazy;
        now += avgNow;
    }
    printf("Lazy: %.2f µs, immediate: %.2f µs\n", lazy, now);
}
