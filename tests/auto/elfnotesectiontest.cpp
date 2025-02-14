/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <elf/elffile.h>
#include <elf/elfnotesection.h>
#include <elf/elfnoteentry.h>

#include <QtTest/qtest.h>
#include <QObject>

#include <elf.h>

class ElfNoteSectionTest : public QObject
{
    Q_OBJECT

    void checkLinuxABISection(const ElfFile& f)
    {
        const auto index = f.indexOfSection(".note.ABI-tag");
        QVERIFY(index > 0);

        ElfNoteSection *section = f.section<ElfNoteSection>(index);
        QVERIFY(section);

        QCOMPARE(section->entryCount(), 1);
        const auto entry = section->entry(0);

        QVERIFY(entry);
        QCOMPARE(entry->section(), section);
        QCOMPARE(entry->size(), section->size());
        QCOMPARE(entry->type(), (uint64_t)NT_GNU_ABI_TAG);
        QCOMPARE(entry->name(), "GNU");
        QVERIFY(entry->isGNUVendorNote());
        QCOMPARE(entry->descriptionSize(), (uint64_t)16);
    }

    void checkFreeBSDABISection(const ElfFile& f)
    {
        const auto index = f.indexOfSection(".note.tag");
        QVERIFY(index > 0);

        ElfNoteSection *section = f.section<ElfNoteSection>(index);
        QVERIFY(section);

        QVERIFY(section->entryCount() > 0);
        const auto entry = section->entry(0);

        QVERIFY(entry);
        QCOMPARE(entry->section(), section);
        QVERIFY(entry->size() <= section->size());
        QCOMPARE(entry->type(), (uint64_t)NT_GNU_ABI_TAG);
        QCOMPARE(entry->name(), "FreeBSD");
        QVERIFY(!entry->isGNUVendorNote());
        QCOMPARE(entry->descriptionSize(), (uint64_t)4);
    }

private Q_SLOTS:
    void testABISection()
    {
        ElfFile f(QStringLiteral(BINDIR "single-executable"));
        QVERIFY(f.open(QFile::ReadOnly));
        QCOMPARE(f.isValid(), true);

#ifdef Q_OS_FREEBSD
        checkFreeBSDABISection(f);
#else
        checkLinuxABISection(f);
#endif
    }
};

QTEST_MAIN(ElfNoteSectionTest)

#include "elfnotesectiontest.moc"
