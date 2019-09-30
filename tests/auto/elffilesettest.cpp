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

#include <elf/elffileset.h>

#include <QtTest/qtest.h>
#include <QObject>

#include <elf.h>

class ElfFileSetTest : public QObject
{
    Q_OBJECT
private slots:
    void testFindDependencies_data()
    {
        QTest::addColumn<QString>("executable");
        QTest::newRow("single-executable") << QStringLiteral(BINDIR "single-executable");
        QTest::newRow("structures") << QStringLiteral(BINDIR "structures");
        QTest::newRow("elf-dissector") << QStringLiteral(BINDIR "elf-dissector");
    }

    void testFindDependencies()
    {
        QFETCH(QString, executable);

        ElfFileSet f;
        f.addFile(executable);
        QVERIFY(f.size() > 1);
    }

    void testInvalid_data()
    {
        QTest::addColumn<QString>("executable");
        QTest::newRow("not existing") << QStringLiteral("not-existing");
        QTest::newRow("text file") << QStringLiteral(BINDIR "../CMakeCache.txt");
    }

    void testInvalid()
    {
        QFETCH(QString, executable);

        ElfFileSet f;
        f.addFile(executable);
        QCOMPARE(f.size(), 0);
    }

    void testFindQt()
    {
        ElfFileSet f;
        f.addFile(QStringLiteral(BINDIR "elf-dissector"));
        bool foundQtCore = false;
        for (int i = 1; i < f.size(); ++i) {
            ElfFile *file = f.file(i);
            QVERIFY(file->isValid());
            QVERIFY(file->dynamicSection());
            if (file->dynamicSection() && file->dynamicSection()->soName() == "libQt5Core.so.5")
                foundQtCore = true;
        }
        QVERIFY(foundQtCore);
    }
};

QTEST_MAIN(ElfFileSetTest)

#include "elffilesettest.moc"
