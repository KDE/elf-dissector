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

#include <dwarf/dwarfexpression.h>

#include <QDebug>
#include <QtTest/qtest.h>
#include <QObject>

class DwarfExpressionTest : public QObject
{
    Q_OBJECT
private slots:
    void testDisplayString_data()
    {
        QTest::addColumn<QByteArray>("block");
        QTest::addColumn<QString>("display");

        QTest::newRow("empty") << QByteArray() << QString("<empty>");
        QTest::newRow("virtual inheritance 24") << QByteArray("\x12\x06\x48\x1c\x06\x22") << QString("DW_OP_dup DW_OP_deref DW_OP_lit24 DW_OP_minus DW_OP_deref DW_OP_plus");
        QTest::newRow("virtual inheritance 32") << QByteArray("\x12\x06\x08\x20\x1c\x06\x22") << QString("DW_OP_dup DW_OP_deref DW_OP_const1u 0x20 DW_OP_minus DW_OP_deref DW_OP_plus");
        QTest::newRow("vtable elem offset 0") << QByteArray("\x10\x00") << QString("DW_OP_constu 0x0");
        QTest::newRow("vtable elem offset 4") << QByteArray("\x10\x04") << QString("DW_OP_constu 0x4");
    }

    void testDisplayString()
    {
        QFETCH(QByteArray, block);
        QFETCH(QString, display);

        DwarfExpression exp((void*)block.constData(), block.size());
        QCOMPARE(exp.displayString(), display);
    }
};

QTEST_MAIN(DwarfExpressionTest)

#include "dwarfexpressiontest.moc"
