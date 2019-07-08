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
#include <elf/elfsymboltablesection.h>
#include <ui/elfmodel/elfmodel.h>

#include <QtTest/qtest.h>
#include <QAbstractItemModelTester>
#include <QObject>

#include <elf.h>

class ElfModelTest : public QObject
{
    Q_OBJECT
private slots:
    void modelTest()
    {
        ElfFileSet s;
        s.addFile(QStringLiteral(BINDIR "single-executable"));
        QVERIFY(s.size() > 1);

        ElfModel model;
        QAbstractItemModelTester modelTest(&model);

        model.setFileSet(&s);

        QCOMPARE(model.rowCount(), s.size());
    }

    void navigationTest()
    {
        ElfFileSet s;
        s.addFile(QStringLiteral(BINDIR "elf-dissector"));
        QVERIFY(s.size() > 1);

        ElfModel model;
        model.setFileSet(&s);

        auto file = s.file(0);
        auto symTab = file->symbolTable();
        for (uint i = 0; i < symTab->header()->entryCount(); ++i) {
            auto idx = model.indexForNode(symTab->entry(i));
            QVERIFY(idx.isValid());
            if (strcmp(symTab->entry(i)->name(), "") == 0)
                continue;
            QCOMPARE(idx.data(Qt::DisplayRole).toString(), QString(symTab->entry(i)->name()));
            auto url = idx.data(ElfModel::NodeUrl).toUrl();
            QVERIFY(url.isValid());
            QVERIFY(!url.path().isEmpty());
            QVERIFY(!url.scheme().isEmpty());

            auto idx2 = model.indexForUrl(url);
            QCOMPARE(idx, idx2);
        }
    }
};

QTEST_MAIN(ElfModelTest)

#include "elfmodeltest.moc"
