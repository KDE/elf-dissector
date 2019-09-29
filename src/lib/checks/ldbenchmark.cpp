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

#include "ldbenchmark.h"

#include <elf/elffile.h>
#include <elf/elffileset.h>

#include <QDebug>
#include <QProcess>

#include <algorithm>
#include <iostream>

#include <cassert>

static double median(QVector<double> data)
{
    if (data.size() == 0)
        return 0.0;
    std::sort(data.begin(), data.end());
    return data.at(data.size() / 2);
}

static double min(const QVector<double> &data)
{
    if (data.size() == 0)
        return 0.0;
    return *std::min_element(data.constBegin(), data.constEnd());
}

static double max(const QVector<double> &data)
{
    if (data.size() == 0)
        return 0.0;
    return *std::max_element(data.constBegin(), data.constEnd());
}


void LDBenchmark::measureFileSet(ElfFileSet* fileSet)
{
    m_fileSet = fileSet;

    m_results.clear();
    m_results.reserve(fileSet->size());

    m_args.reserve(fileSet->size() + 1);
    m_args.push_back(QString()); // placeholder for mode argument

    for (int i = fileSet->size() - 1; i >= 0; --i) {
        const auto fileName = fileSet->file(i)->fileName();
        m_args.push_back(fileName);
        Result r;
        r.fileName = fileName.toUtf8();
        m_results.push_back(r);
    }

    measure(LoadMode::None, 1); // avoid cold cache skewing the results
    measure(LoadMode::Lazy, 5);
    measure(LoadMode::Now, 5);
}

void LDBenchmark::measure(LDBenchmark::LoadMode mode, int iterations)
{
    m_args[0] = mode == LoadMode::Lazy ? QStringLiteral("RTLD_LAZY") : QStringLiteral("RTLD_NOW");
    for (int i = 0; i < iterations; ++i) {
        QProcess proc;
        proc.setProcessChannelMode(QProcess::QProcess::ForwardedErrorChannel);
        proc.start(QStringLiteral("ldbenchmark-runner"), m_args); // TODO find in libexec
        proc.waitForFinished();
        if (proc.exitStatus() == QProcess::CrashExit)
            qWarning() << "Benchmark runner crashed!";
        readResults(&proc, mode);
    }
}

void LDBenchmark::readResults(QProcess* proc, LoadMode mode)
{
    while(proc->canReadLine()) {
        const auto line = proc->readLine();
        if (!line.startsWith("LDBENCHMARKRUNNER\t")) {
            qDebug() << "target stdout:" << line;
            continue;
        }
        const auto index = line.lastIndexOf('\t');
        const auto fileName = line.mid(18, index - 18);
        const auto cost = line.mid(index).trimmed().toDouble();
        auto it = std::find_if(m_results.begin(), m_results.end(), [fileName](const Result &res) {
            return res.fileName == fileName;
        });
        assert(it != m_results.end());

        switch (mode) {
            case LoadMode::Lazy:
                (*it).lazy.push_back(cost);
                break;
            case LoadMode::Now:
                (*it).now.push_back(cost);
                break;
            case LoadMode::None:
                return;
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
        const auto file = m_fileSet->file(m_results.size() - 1 - i);
        f.write(file->displayName().toUtf8());
        f.write("\t");
        f.write(QByteArray::number(::median(res.lazy)));
        f.write("\t");
        f.write(QByteArray::number(::min(res.lazy)));
        f.write("\t");
        f.write(QByteArray::number(::max(res.lazy)));
        f.write("\t");
        f.write(QByteArray::number(::median(res.now)));
        f.write("\t");
        f.write(QByteArray::number(::min(res.now)));
        f.write("\t");
        f.write(QByteArray::number(::max(res.now)));
        f.write("\n");
    }
}

int LDBenchmark::size() const
{
    return m_results.size();
}

double LDBenchmark::median(LoadMode mode, int index) const
{
    const auto &res = m_results.at(index);
    return ::median(mode == LoadMode::Lazy ? res.lazy : res.now);
}

double LDBenchmark::min(LDBenchmark::LoadMode mode, int index) const
{
    const auto &res = m_results.at(index);
    return ::min(mode == LoadMode::Lazy ? res.lazy : res.now);
}

ElfFile* LDBenchmark::file(int index) const
{
    return m_fileSet->file(size() - index - 1);
}
