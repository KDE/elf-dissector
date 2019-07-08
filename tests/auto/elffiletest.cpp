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

#include <elf/elffile.h>
#include <elf/elfsymboltablesection.h>
#include <elf/elfheader.h>
#include <elf/elfpltsection.h>
#include <elf/elfrelocationsection.h>
#include <elf/elfgotsection.h>

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
        QTest::newRow("single-executable") << QStringLiteral(BINDIR "single-executable");
        QTest::newRow("structures") << QStringLiteral(BINDIR "structures");
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
        QCOMPARE((uint16_t)f.sectionCount(), f.header()->sectionHeaderCount());
        QVERIFY(f.size() > 0);
        QVERIFY(f.indexOfSection(".dynsym") >= 0);
        QCOMPARE(f.indexOfSection(".doesnotexist"), -1);

        QVERIFY(f.dynamicSection());
        QVERIFY(f.dynamicSection()->size() > 0);
        QVERIFY(f.symbolTable());
        QVERIFY(f.symbolTable()->size() > 0);

        if (f.indexOfSection(".plt") > 0) {
            auto pltSection = f.section<ElfPltSection>(f.indexOfSection(".plt"));
            QVERIFY(pltSection);
            QVERIFY(pltSection->header()->entryCount() > 0);
            QVERIFY(pltSection->gotSection());
            for (uint i = 1; i < pltSection->header()->entryCount(); ++i) {
                auto pltEntry = pltSection->entry(i);
                QVERIFY(pltEntry);
                QVERIFY(pltEntry->gotEntry());
            }
        }

        QVERIFY(f.reverseRelocator());

        for (int i = 0; i < f.header()->sectionHeaderCount(); ++i) {
            auto shdr = f.sectionHeaders().at(i);
            if (shdr->type() == SHT_REL || shdr->type() == SHT_RELA) {
                auto section = f.section<ElfRelocationSection>(i);
                QVERIFY(section);
                QVERIFY(section->header()->entryCount() > 0);
            }

            if (QByteArray(shdr->name()).startsWith(".got")) {
                auto section = f.section<ElfGotSection>(i);
                QVERIFY(section);
                uint startIndex = 0;
                if (strcmp(shdr->name(), ".got.plt") == 0)
                    startIndex = 3; // the first 3 entries are placeholders for lazy symbol resolution
                for (uint i = startIndex; i < section->header()->entryCount(); ++i) {
                    auto gotEntry = section->entry(i);
                    QVERIFY(f.reverseRelocator()->find(gotEntry->address()));
                }
            }
        }

        QVERIFY(f.buildId().size() >= 16);

        QCOMPARE((uint16_t)f.segmentHeaders().size(), f.header()->programHeaderCount());
    }

    void testFailedLoad_data()
    {
        QTest::addColumn<QString>("executable");
        QTest::newRow("not existing") << QStringLiteral("not-existing");
        QTest::newRow("text file") << QStringLiteral(BINDIR "../CMakeCache.txt");
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
