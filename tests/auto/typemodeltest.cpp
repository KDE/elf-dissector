/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <elf/elffileset.h>
#include <ui/typemodel/typemodel.h>

#include <QtTest/qtest.h>
#include <QAbstractItemModelTester>
#include <QObject>

class TypeModelTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void modelTest_data()
    {
        QTest::addColumn<QString>("file");
        QTest::newRow("simple") << QStringLiteral(BINDIR "structures");
        QTest::newRow("complex") << QStringLiteral(BINDIR "elf-dissector");
    }

    void modelTest()
    {
        QFETCH(QString, file);
        ElfFileSet s;
        s.addFile(file);
        QVERIFY(s.size() > 1);

        TypeModel model;
        QAbstractItemModelTester modelTest(&model);

        model.setFileSet(&s);

        QVERIFY(model.rowCount() > 0);
    }
};

QTEST_MAIN(TypeModelTest)

#include "typemodeltest.moc"
