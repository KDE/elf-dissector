/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <elf/elffileset.h>
#include <ui/dependencymodel/dependencymodel.h>

#include <QtTest/qtest.h>
#include <QAbstractItemModelTester>
#include <QObject>

class DependencyModelTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void modelTest_data()
    {
        QTest::addColumn<QString>("file");
        QTest::newRow("simple") << QStringLiteral(BINDIR "single-executable");
        QTest::newRow("complex") << QStringLiteral(BINDIR "elf-dissector");
    }

    void modelTest()
    {
        QFETCH(QString, file);
        ElfFileSet s;
        s.addFile(file);
        QVERIFY(s.size() > 1);

        DependencyModel model;
        QAbstractItemModelTester modelTest(&model);

        model.setFileSet(&s);

        QCOMPARE(model.rowCount(), 1);

        const auto topIndex = model.index(0, 0);
        QVERIFY(model.rowCount(topIndex) > 0);
        QVERIFY(topIndex.isValid());
        QCOMPARE(model.rowCount(topIndex), s.file(0)->dynamicSection()->neededLibraries().size());
    }
};

QTEST_MAIN(DependencyModelTest)

#include "dependencymodeltest.moc"
