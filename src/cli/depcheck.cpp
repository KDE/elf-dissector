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

    foreach (const auto &fileName, parser.positionalArguments()) {
        ElfFileSet set;
        set.addFile(fileName);
        if (set.size() == 0)
            continue;
        const auto unusedDeps = DependenciesCheck::unusedDependencies(&set, parser.isSet(recursiveOption) ? -1 : 0);
        DependenciesCheck::printUnusedDependencies(&set, unusedDeps);
    }

    return 0;
}
