/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef GNUPLOTTER_H
#define GNUPLOTTER_H

#include <QSize>
#include <QString>

#include <memory>

class QTemporaryDir;

/** Turn data into a plot using a gnuplot driver template file. */
class Gnuplotter
{
public:
    explicit Gnuplotter();
    Gnuplotter(const Gnuplotter& other) = delete;
    Gnuplotter(Gnuplotter &&other);
    ~Gnuplotter();

    Gnuplotter& operator=(const Gnuplotter &other) = delete;
    Gnuplotter& operator=(Gnuplotter &&other);

    bool isValid() const;

    void setSize(const QSize &size);
    void setTemplate(const QString &templateFileName);

    QString workingDir() const;
    QString imageFileName() const;

    void plot() const;

    static bool hasGnuplot();

private:
    void processTemplate() const;

    QString m_templateFileName;
    QSize m_outputSize = { 1024, 786 };
    // QTemporaryDir is not movable :-(
    std::unique_ptr<QTemporaryDir> m_tempDir;
};

#endif // GNUPLOTTER_H
