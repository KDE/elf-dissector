/*
    Copyright (C) 2013-2014 Volker Krause <vkrause@kde.org>

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

#include <config-elf-dissector-version.h>

#include <ui/mainwindow.h>
#include <printers/dwarfprinter.h>

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName(QStringLiteral("ELF Dissector"));
    QGuiApplication::setApplicationDisplayName(MainWindow::tr("ELF Dissector"));
    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QCoreApplication::setApplicationVersion(QStringLiteral(ELF_DISSECTOR_VERSION_STRING));
    QGuiApplication::setDesktopFileName(QStringLiteral("org.kde.elf-dissector"));

    Q_INIT_RESOURCE(plotter);
    DwarfPrinter::registerConverterFunctions();

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(QStringLiteral("elf"), MainWindow::tr("ELF file to open"), QStringLiteral("<elf>"));
    parser.process(app);

    MainWindow mainWindow;
    mainWindow.show();
    foreach (const QString &file, parser.positionalArguments())
        mainWindow.loadFile(file);

    return app.exec();
}
