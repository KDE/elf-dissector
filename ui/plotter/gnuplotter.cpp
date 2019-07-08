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

#include "gnuplotter.h"

#include <QDebug>
#include <QGuiApplication>
#include <QPalette>
#include <QProcess>
#include <QStandardPaths>
#include <QTemporaryDir>

Gnuplotter::Gnuplotter() :
    m_tempDir(new QTemporaryDir)
{
}

Gnuplotter::~Gnuplotter() = default;

Gnuplotter::Gnuplotter(Gnuplotter&&) = default;
Gnuplotter& Gnuplotter::operator=(Gnuplotter&&) = default;

bool Gnuplotter::isValid() const
{
    return !m_templateFileName.isEmpty() && !m_outputSize.isNull();
}

bool Gnuplotter::hasGnuplot()
{
    return !QStandardPaths::findExecutable(QStringLiteral("gnuplot")).isEmpty();
}

void Gnuplotter::setTemplate(const QString& templateFileName)
{
    m_templateFileName = templateFileName;
}

void Gnuplotter::setSize(const QSize& size)
{
    m_outputSize = size;
}

QString Gnuplotter::workingDir() const
{
    return m_tempDir->path();
}

QString Gnuplotter::imageFileName() const
{
    return m_tempDir->path() + "/plot.png";
}

void Gnuplotter::plot() const
{
    // TODO error handling
    processTemplate();
    QProcess proc;
    proc.setProcessChannelMode(QProcess::ForwardedChannels);
    proc.setWorkingDirectory(m_tempDir->path());
    proc.start(QStandardPaths::findExecutable(QStringLiteral("gnuplot")), { QStringLiteral("plot.gnuplot") });
    proc.waitForFinished();
}

void Gnuplotter::processTemplate() const
{
    // TODO error handling

    QFile out(m_tempDir->path() + "/plot.gnuplot");
    if (!out.open(QFile::WriteOnly | QFile::Truncate)) {
        qWarning() << "Failed to write to" << m_tempDir->path() << ":" << out.errorString();
        return;
    }

    out.write("set terminal png transparent size ");
    out.write(QByteArray::number(m_outputSize.width()));
    out.write(",");
    out.write(QByteArray::number(m_outputSize.height()));
    out.write("\nset output \"plot.png\"\n");

    QFile in(m_templateFileName);
    if (!in.open(QFile::ReadOnly)) {
        qWarning() << "Failed to read" << m_templateFileName << ":" << in.errorString();
        return;
    }

    auto t = in.readAll();
    t.replace("@TEXTCOLOR@", QGuiApplication::palette().color(QPalette::Text).name().toUtf8());

    out.write(t);
}
