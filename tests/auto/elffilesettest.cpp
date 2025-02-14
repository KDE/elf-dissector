/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <elf/elffileset.h>

#include <QtTest/qtest.h>
#include <QObject>

#include <elf.h>

class ElfFileSetTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
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
            if (file->dynamicSection() && file->dynamicSection()->soName() == "libQt6Core.so.6")
                foundQtCore = true;
        }
        QVERIFY(foundQtCore);
    }
};

QTEST_MAIN(ElfFileSetTest)

#include "elffilesettest.moc"
