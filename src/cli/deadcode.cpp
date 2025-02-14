/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <config-elf-dissector-version.h>

#include <elf/elffileset.h>
#include <checks/deadcodefinder.h>

#include <QCoreApplication>
#include <QCommandLineParser>

int main(int argc, char** argv)
{
    QCoreApplication::setApplicationName(QStringLiteral("ELF Dead Code Finder"));
    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QCoreApplication::setApplicationVersion(QStringLiteral(ELF_DISSECTOR_VERSION_STRING));

    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption excludePrefixOpt(QStringLiteral("exclude-prefix"), QStringLiteral("Exclude ELF files in this prefix."), QStringLiteral("exclude"));
    parser.addOption(excludePrefixOpt);
    parser.addPositionalArgument(QStringLiteral("elf"), QStringLiteral("ELF objects to analyze"), QStringLiteral("<elf>"));
    parser.process(app);

    ElfFileSet set;
    for (const auto &fileName : parser.positionalArguments()) {
        set.addFile(fileName);
    }

    DeadCodeFinder finder;
    if (parser.isSet(excludePrefixOpt))
        finder.setExcludePrefixes(parser.values(excludePrefixOpt));

    finder.findUnusedSymbols(&set);
    finder.dumpResults();

    return 0;
}
