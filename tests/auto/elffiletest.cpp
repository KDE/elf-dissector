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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <elf/elffile.h>
#include <elf/elfsymboltablesection.h>
#include <elf/elfheader.h>

#include <QtTest/qtest.h>
#include <QObject>

#include <elf.h>

class ElfFileTest : public QObject
{
    Q_OBJECT
private slots:
    void testLoad_data()
    {
        QTest::addColumn<QString>("executable");
        QTest::newRow("single-executable") << QString(BINDIR "single-executable");
        QTest::newRow("structures") << QString(BINDIR "structures");
    }
    void testLoad()
    {
        QFETCH(QString, executable);

        ElfFile f(executable);
        QVERIFY(f.open(QFile::ReadOnly));
        QCOMPARE(f.isValid(), true);
        QCOMPARE(f.byteOrder(), ELFDATA2LSB);

        QVERIFY(f.header());
        QVERIFY(f.header()->programHeaderCount() > 0);
        QVERIFY(f.header()->programHeaderEntrySize() > 0);
        QVERIFY(f.header()->programHeaderTableOffset() > 0);
        QVERIFY(f.header()->sectionHeaderCount() > 0);
        QVERIFY(f.header()->sectionHeaderEntrySize() > 0);
        QVERIFY(f.header()->sectionHeaderTableOffset() > 0);

        QCOMPARE(f.sectionHeaders().isEmpty(), false);
        QCOMPARE((uint16_t)f.sectionHeaders().size(), f.header()->sectionHeaderCount());
        QVERIFY(f.size() > 0);
        QVERIFY(f.indexOfSection(".dynsym") >= 0);
        QCOMPARE(f.indexOfSection(".doesnotexist"), -1);

        QVERIFY(f.dynamicSection());
        QVERIFY(f.dynamicSection()->size() > 0);
        QVERIFY(f.symbolTable());
        QVERIFY(f.symbolTable()->size() > 0);
    }

    void testFailedLoad_data()
    {
        QTest::addColumn<QString>("executable");
        QTest::newRow("not existing") << QString("not-existing");
        QTest::newRow("text file") << QString(BINDIR "../CMakeCache.txt");
    }

    void testFailedLoad()
    {
        QFETCH(QString, executable);

        ElfFile f(executable);
        QCOMPARE(f.open(QFile::ReadOnly), false);
        QCOMPARE(f.isValid(), false);
    }
};

QTEST_MAIN(ElfFileTest)

#include "elffiletest.moc"
