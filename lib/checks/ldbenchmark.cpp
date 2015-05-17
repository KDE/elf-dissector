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

#include <algorithm>
#include <iostream>

#include <cassert>

static double average(const QVector<double> &data)
{
    if (data.size() == 0)
        return 0.0;
    const double sum = std::accumulate(data.constBegin(), data.constEnd(), 0.0);
    qDebug() << sum << data << data.size() << (sum / data.size());
    return sum / data.size();
}

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

//     dumpResults();
}

void LDBenchmark::measure(LDBenchmark::LoadMode mode, int iterations)
{
    m_args[0] = mode == LoadMode::Lazy ? "RTLD_LAZY" : "RTLD_NOW";
    for (int i = 0; i < iterations; ++i) {
        QProcess proc;
        proc.setProcessChannelMode(QProcess::QProcess::ForwardedErrorChannel);
        proc.start("ldbenchmark-runner", m_args); // TODO find in libexec
        proc.waitForFinished();
        readResults(&proc, mode);
    }
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
        f.write(QByteArray::number(median(res.lazy)));
        f.write("\t");
        f.write(QByteArray::number(min(res.lazy)));
        f.write("\t");
        f.write(QByteArray::number(max(res.lazy)));
        f.write("\t");
        f.write(QByteArray::number(median(res.now)));
        f.write("\t");
        f.write(QByteArray::number(min(res.now)));
        f.write("\t");
        f.write(QByteArray::number(max(res.now)));
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
        printf("%s\t%.2f\t%.2f\n", res.fileName.constData(), average(res.lazy), average(res.now));
        lazy += average(res.lazy);
        now += average(res.now);
    }
    printf("Lazy: %.2f µs, immediate: %.2f µs\n", lazy, now);
}
