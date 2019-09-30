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

#include <dwarf/dwarfexpression.h>

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

        // real-world examples
        QTest::newRow("empty") << QByteArray() << QStringLiteral("<empty>");
        QTest::newRow("virtual inheritance 24") << QByteArray("\x12\x06\x48\x1c\x06\x22") << QStringLiteral("DW_OP_dup DW_OP_deref DW_OP_lit24 DW_OP_minus DW_OP_deref DW_OP_plus");
        QTest::newRow("virtual inheritance 32") << QByteArray("\x12\x06\x08\x20\x1c\x06\x22") << QStringLiteral("DW_OP_dup DW_OP_deref DW_OP_const1u 32 DW_OP_minus DW_OP_deref DW_OP_plus");
        QTest::newRow("virtual inheritance 112") << QByteArray("\x12\x06\x08\x70\x1c\x06\x22") << QStringLiteral("DW_OP_dup DW_OP_deref DW_OP_const1u 112 DW_OP_minus DW_OP_deref DW_OP_plus");
        QTest::newRow("vtable elem offset 0") << QByteArray("\x10\x00", 2) << QStringLiteral("DW_OP_constu 0");
        QTest::newRow("vtable elem offset 4") << QByteArray("\x10\x04") << QStringLiteral("DW_OP_constu 4");
        QTest::newRow("address") << QByteArray("\x03\x34\x08\x40\x00\x00\x00\x00\x00", 9) << QStringLiteral("DW_OP_addr 0x400834");
        QTest::newRow("argument location") << QByteArray("\x91\x88\x7f") << QStringLiteral("DW_OP_fbreg -120");

        // artificial
        QTest::newRow("OP_const1s") << QByteArray("\x09\xfe") << QStringLiteral("DW_OP_const1s -2");
        QTest::newRow("OP_const2u") << QByteArray("\x0a\x2a\x00", 3) << QStringLiteral("DW_OP_const2u 42");
        QTest::newRow("OP_const4s") << QByteArray("\x0d\xfd\xff\xff\xff") << QStringLiteral("DW_OP_const4s -3");
        QTest::newRow("OP_const8u") << QByteArray("\x0e\x34\x08\x40\x00\x00\x00\x00\x00", 9) << QStringLiteral("DW_OP_const8u 4196404");
    }

    void testDisplayString()
    {
        QFETCH(QByteArray, block);
        QFETCH(QString, display);

        DwarfExpression exp((void*)block.constData(), block.size(), 8);
        QCOMPARE(exp.displayString(), display);
    }

    void testSimpleEval_data()
    {
        QTest::addColumn<QByteArray>("block");
        QTest::addColumn<int>("result");

        QTest::newRow("OP_addr") << QByteArray("\x03\x34\x08\x40\x00\x00\x00\x00\x00", 9) << 0x400834;
        QTest::newRow("OP_const1s") << QByteArray("\x09\xfe") << -2;
        QTest::newRow("OP_const2u") << QByteArray("\x0a\x2a\x00", 3) << 42;
        QTest::newRow("OP_const4s") << QByteArray("\x0d\xfd\xff\xff\xff") << -3;
        QTest::newRow("OP_const8u") << QByteArray("\x0e\x34\x08\x40\x00\x00\x00\x00\x00", 9) << 4196404;
        QTest::newRow("OP_constu") << QByteArray("\x10\x00", 2) << 0;
        QTest::newRow("OP_constu") << QByteArray("\x10\x04") << 4;

        QTest::newRow("OP_lit23 OP_dup OP_plus") << QByteArray("\x47\x12\x22") << 46;
        QTest::newRow("OP_const1s 42 OP_lit20 OP_minus") << QByteArray("\x09\x2a\x44\x1c") << 22;
    }

    void testSimpleEval()
    {
        QFETCH(QByteArray, block);
        QFETCH(int, result);

        DwarfExpression exp((void*)block.constData(), block.size(), 8);
        QVERIFY(exp.evaluateSimple());
        QCOMPARE(exp.top(), (uint64_t)result);
    }
};

QTEST_MAIN(DwarfExpressionTest)

#include "dwarfexpressiontest.moc"
