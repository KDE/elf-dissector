/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
    for (const QString &file : parser.positionalArguments())
        mainWindow.loadFile(file);

    return app.exec();
}
