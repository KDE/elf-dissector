/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "config-elf-dissector.h"
#include <elf/elffile.h>
#include <elf/elfgnuhashsection.h>
#include <elf/elfhashsection.h>
#include <elf/elfsymboltablesection.h>

#include <QtTest/qtest.h>
#include <QObject>

#include <elf.h>
#include <cstring>

class ElfHashTest: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testHashSection()
    {
        ElfFile f(QStringLiteral(BINDIR "/elf-dissector"));
        QVERIFY(f.open(QFile::ReadOnly));
        QVERIFY(f.isValid());

        const auto hashIndex = f.indexOfSection(SHT_HASH);
        const auto gnuHashIndex = f.indexOfSection(SHT_GNU_HASH);
        QVERIFY(hashIndex >= 0 || gnuHashIndex >= 0);
        if (hashIndex < 0)
            return;

        const auto hashSection = f.section<ElfHashSection>(hashIndex);
        QVERIFY(hashSection);

        const auto symTab = hashSection->linkedSection<ElfSymbolTableSection>();
        QVERIFY(symTab);

        QVERIFY(hashSection->bucketCount() > 0);
        QCOMPARE((uint64_t)hashSection->chainCount(), symTab->header()->entryCount());
        // EXPECTED_FAIL: the .hash section is broken with --hash-style=both and gold of binutils <= 2.24 (works with the old ld, fixed in 2.25)
#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 25)
        for (uint32_t i = 0; i < symTab->header()->entryCount(); ++i) {
            const auto entry = symTab->entry(i);
            // qt_version_tag is a specially versioned symbol in openSUSE's Qt build thus affecting the KDE CI
            if (std::strcmp(entry->name(), "") == 0 || std::strcmp(entry->name(), "qt_version_tag") == 0) {
                continue;
            }
            QCOMPARE(hashSection->lookup(entry->name()), entry);
        }
#endif

        const auto hist = hashSection->histogram();
        const uint32_t sum = std::accumulate(hist.begin(), hist.end(), 0);
        QCOMPARE(sum, hashSection->bucketCount());
    }

    void testGnuHashSection()
    {
        ElfFile f(QStringLiteral(BINDIR "/elf-dissector"));
        QVERIFY(f.open(QFile::ReadOnly));
        QVERIFY(f.isValid());

        const auto hashIndex = f.indexOfSection(SHT_GNU_HASH);
        const auto sysvHashIndex = f.indexOfSection(SHT_HASH);
        QVERIFY(hashIndex >= 0 || sysvHashIndex >= 0);
        if (hashIndex < 0)
            return;

        const auto hashSection = f.section<ElfGnuHashSection>(hashIndex);
        QVERIFY(hashSection);

        const auto symTab = hashSection->linkedSection<ElfSymbolTableSection>();
        QVERIFY(symTab);

        QVERIFY(hashSection->bucketCount() > 0);
        QVERIFY(hashSection->symbolIndex() > 0);
        QVERIFY(hashSection->symbolIndex() < symTab->header()->entryCount());
        QCOMPARE((uint64_t)hashSection->chainCount(), symTab->header()->entryCount() - hashSection->symbolIndex());

        for (uint32_t i = hashSection->symbolIndex(); i < symTab->header()->entryCount(); ++i) {
            const auto entry = symTab->entry(i);
            // qt_version_tag is a specially versioned symbol in openSUSE's Qt build thus affecting the KDE CI
            if (std::strcmp(entry->name(), "qt_version_tag") == 0) {
                continue;
            }
            QCOMPARE(hashSection->lookup(entry->name()), entry);
        }

        const auto hist = hashSection->histogram();
        const uint32_t sum = std::accumulate(hist.begin(), hist.end(), 0);
        QCOMPARE(sum, hashSection->bucketCount());
    }
};

QTEST_MAIN(ElfHashTest)

#include "elfhashtest.moc"
