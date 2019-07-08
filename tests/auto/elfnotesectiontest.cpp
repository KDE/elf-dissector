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

#include <elf/elffile.h>
#include <elf/elfnotesection.h>
#include <elf/elfnoteentry.h>

#include <QtTest/qtest.h>
#include <QObject>

#include <elf.h>

class ElfNoteSectionTest : public QObject
{
    Q_OBJECT
private slots:
    void testABISection()
    {
        ElfFile f(QStringLiteral(BINDIR "single-executable"));
        QVERIFY(f.open(QFile::ReadOnly));
        QCOMPARE(f.isValid(), true);

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
};

QTEST_MAIN(ElfNoteSectionTest)

#include "elfnotesectiontest.moc"
