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
#include <ui/typemodel/typemodel.h>

#include <QtTest/qtest.h>
#include <QAbstractItemModelTester>
#include <QObject>

class TypeModelTest : public QObject
{
    Q_OBJECT
private slots:
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
