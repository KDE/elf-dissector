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
