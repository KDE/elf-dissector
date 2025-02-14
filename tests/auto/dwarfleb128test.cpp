/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <dwarf/dwarfleb128.h>

#include <QtTest/qtest.h>
#include <QObject>

class DwarfLEB128Test : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testDecodeULEB128_data()
    {
        QTest::addColumn<QByteArray>("data");
        QTest::addColumn<uint64_t>("result");
        QTest::addColumn<int>("size");

        QTest::newRow("0") << QByteArray("\x00") << uint64_t(0) << 1;
        QTest::newRow("42") << QByteArray("\x2A") << uint64_t(42) << 1;
        QTest::newRow("624485") << QByteArray("\xE5\x8E\x26") << uint64_t(624485) << 3;
    }

    void testDecodeULEB128()
    {
        QFETCH(QByteArray, data);
        QFETCH(uint64_t, result);
        QFETCH(int, size);

        int decodedSize = 0;
        QCOMPARE(DwarfLEB128::decodeUnsigned(data.constData(), &decodedSize), result);
        QCOMPARE(decodedSize, size);
        QCOMPARE(DwarfLEB128::decodeUnsigned(data.constData()), result);
    }

    void testDecodeSLEB128_data()
    {
        QTest::addColumn<QByteArray>("data");
        QTest::addColumn<int64_t>("result");
        QTest::addColumn<int>("size");

        QTest::newRow("0") << QByteArray("\x00") << int64_t(0) << 1;
        QTest::newRow("42") << QByteArray("\x2A") << int64_t(42) << 1;
        QTest::newRow("-624485") << QByteArray("\x9b\xf1\x59") << int64_t(-624485) << 3;
    }

    void testDecodeSLEB128()
    {
        QFETCH(QByteArray, data);
        QFETCH(int64_t, result);
        QFETCH(int, size);

        int decodedSize = 0;
        QCOMPARE(DwarfLEB128::decodeSigned(data.constData(), &decodedSize), result);
        QCOMPARE(decodedSize, size);
        QCOMPARE(DwarfLEB128::decodeSigned(data.constData()), result);
    }
};

QTEST_MAIN(DwarfLEB128Test)

#include "dwarfleb128test.moc"
