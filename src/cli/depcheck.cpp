/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <config-elf-dissector-version.h>

#include <checks/dependenciescheck.h>

#include <elf/elffileset.h>

#include <QCoreApplication>
#include <QCommandLineParser>

int main(int argc, char** argv)
{
    QCoreApplication::setApplicationName(QStringLiteral("ELF Dissector"));
    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QCoreApplication::setApplicationVersion(QStringLiteral(ELF_DISSECTOR_VERSION_STRING));

    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption recursiveOption(QStringList() << QStringLiteral("r") << QStringLiteral("recursive"), QStringLiteral("Check all dependencies recursively as well"));
    parser.addOption(recursiveOption);
    parser.addPositionalArgument(QStringLiteral("elf"), QStringLiteral("ELF library to open"), QStringLiteral("<elf>"));
    parser.process(app);

    for (const auto &fileName : parser.positionalArguments()) {
        ElfFileSet set;
        set.addFile(fileName);
        if (set.size() == 0)
            continue;
        const auto unusedDeps = DependenciesCheck::unusedDependencies(&set, parser.isSet(recursiveOption) ? -1 : 0);
        DependenciesCheck::printUnusedDependencies(&set, unusedDeps);
    }

    return 0;
}
