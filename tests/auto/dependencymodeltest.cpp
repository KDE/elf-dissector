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
#include <ui/dependencymodel/dependencymodel.h>

#include <QtTest/qtest.h>
#include <QAbstractItemModelTester>
#include <QObject>

class DependencyModelTest : public QObject
{
    Q_OBJECT
private slots:
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
