/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <dwarf/dwarfdie.h>
#include <dwarf/dwarfcudie.h>
#include <dwarf/dwarfinfo.h>
#include <dwarf/dwarfranges.h>
#include <dwarf/dwarfaddressranges.h>

#include <QtTest/qtest.h>
#include <QObject>

#include <dwarf.h>

class DwarfDieTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testCU()
    {
        ElfFile f(QStringLiteral(BINDIR "single-executable"));
        QVERIFY(f.open(QFile::ReadOnly));
        QVERIFY(f.dwarfInfo());
        QVERIFY(f.dwarfInfo()->compilationUnits().size() > 0);

        DwarfDie *cu = nullptr;
        for (auto die : f.dwarfInfo()->compilationUnits()) {
            if (die->name().contains("single-executable")) {
                cu = die;
                break;
            }
        }

        QVERIFY(cu);
        QCOMPARE(cu->tag(), (Dwarf_Half)DW_TAG_compile_unit);
        QVERIFY(cu->children().size() > 0);
        QVERIFY(cu->attributes().size() > 0);
    }

    void testAttribute_AT_ranges()
    {
        ElfFile f(QStringLiteral(BINDIR "single-executable"));
        QVERIFY(f.open(QFile::ReadOnly));
        QVERIFY(f.dwarfInfo());
        QVERIFY(f.dwarfInfo()->compilationUnits().size() > 0);

        for (auto cu : f.dwarfInfo()->compilationUnits()) {
            const auto lowPC = cu->attribute(DW_AT_low_pc);
            const auto ranges = cu->attribute(DW_AT_ranges);
            if (lowPC.isNull() && ranges.isNull())
                continue;

            if (cu->version() < 5) {
                QVERIFY(lowPC.isNull() != ranges.isNull()); // exactly one of them must be present in DWARF version < 5
            }
            if (ranges.userType() == qMetaTypeId<DwarfRanges>()) {
                const auto r = ranges.value<DwarfRanges>();
                QVERIFY(r.isValid());
                QVERIFY(r.size() > 0);
            }
            if (!lowPC.isNull() && lowPC.value<uint64_t>() != 0) {
                const auto textSection = f.section<ElfSection>(f.indexOfSection(".text"));
                QVERIFY(lowPC.value<uint64_t>() >= textSection->header()->virtualAddress());
                QVERIFY(lowPC.value<uint64_t>() < textSection->header()->virtualAddress() + textSection->header()->size());
            }
        }
    }

    void testArange()
    {
        ElfFile f(QStringLiteral(BINDIR "single-executable"));
        QVERIFY(f.open(QFile::ReadOnly));
        QVERIFY(f.dwarfInfo());
        QVERIFY(f.dwarfInfo()->addressRanges()->isValid());

        QList<DwarfDie*> dieQueue;
        dieQueue.resize(f.dwarfInfo()->compilationUnits().size());
        auto cus = f.dwarfInfo()->compilationUnits();
        std::copy(cus.constBegin(), cus.constEnd(), dieQueue.begin());
        while (!dieQueue.isEmpty()) {
            const auto die = dieQueue.takeFirst();
            dieQueue += die->children();

            const auto lowPC = die->attribute(DW_AT_low_pc).toULongLong();
            if (lowPC <= 0)
                continue;

            const auto lookupCU = f.dwarfInfo()->addressRanges()->compilationUnitForAddress(lowPC);
            auto cuDie = die;
            while (cuDie && cuDie->tag() != DW_TAG_compile_unit)
                cuDie = cuDie->parentDie();
            QCOMPARE(cuDie, lookupCU);

            if (die->tag() == DW_TAG_compile_unit)
                continue;

            const auto lookupDie = f.dwarfInfo()->addressRanges()->dieForAddress(lowPC);
            QVERIFY(die == lookupDie || lowPC == lookupDie->attribute(DW_AT_low_pc).toULongLong());
        }
    }
};

QTEST_MAIN(DwarfDieTest)

#include "dwarfdietest.moc"
