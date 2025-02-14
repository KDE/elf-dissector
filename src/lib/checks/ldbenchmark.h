/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef LDBENCHMARK_H
#define LDBENCHMARK_H

#include <QByteArray>
#include <QStringList>
#include <QList>

class QProcess;

class ElfFileSet;
class ElfFile;

/** Load all libraries in dependency order one by one and measure the needed time. */
class LDBenchmark
{
public:
    void measureFileSet(ElfFileSet *fileSet);

    void writeCSV(const QString &fileName);

    /** Number of files we have results for. */
    int size() const;

    enum class LoadMode { None, Now, Lazy };
    double median(LoadMode mode, int index) const;
    double min(LoadMode mode, int index) const;
    ElfFile* file(int index) const;

private:
    void measure(LoadMode mode, int iterations);
    void readResults(QProcess *proc, LoadMode mode);

    ElfFileSet *m_fileSet = nullptr;

    struct Result {
        QByteArray fileName;
        QList<double> lazy;
        QList<double> now;
    };
    QList<Result> m_results;
    QStringList m_args;
};

#endif // LDBENCHMARK_H
