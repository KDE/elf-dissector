/*
    SPDX-FileCopyrightText: 2019 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <elf/elffile.h>
#include <elf/elfsymboltablesection.h>
#include <elf/elfsymboltableentry.h>

#include <QDebug>
#include <QtTest/qtest.h>
#include <QObject>

#include <elf.h>

class ElfSymbolTableTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testSymbolTable_data()
    {
        QTest::addColumn<QString>("executable");
        QTest::newRow("single-executable") << QStringLiteral(BINDIR "single-executable");
        QTest::newRow("structures") << QStringLiteral(BINDIR "structures");
        QTest::newRow("elf-dissector") << QStringLiteral(BINDIR "elf-dissector");
    }

    void testSymbolTable()
    {
        QFETCH(QString, executable);

        ElfFile f(executable);
        QVERIFY(f.open(QFile::ReadOnly));
        QCOMPARE(f.isValid(), true);

        const auto symtab = f.symbolTable();
        QVERIFY(symtab);
        QVERIFY(symtab->header()->entryCount() > 0);
        QVERIFY(symtab->importCount() > 0);
        QVERIFY(symtab->exportCount() >= 0);

        for (uint32_t i = 0; i < symtab->header()->entryCount(); ++i) {
            const auto entry = symtab->entry(i);
            QVERIFY(entry);
            QCOMPARE(i, entry->index());
            QCOMPARE(symtab->entry(i), entry);
            QVERIFY(entry->name());
            if (entry->value() && entry->size()) {
                auto revEntry = symtab->entryWithValue(entry->value());
                QVERIFY(revEntry);
                QCOMPARE(revEntry->value(), entry->value()); // not necessarily entry == revEntry though

                revEntry = symtab->entryContainingValue(entry->value());
                QVERIFY(revEntry);
                QVERIFY(entry->value() >= revEntry->value());
                QVERIFY(entry->value() < revEntry->value() + revEntry->size());

                const auto val = entry->value() + entry->size() - 1;
                revEntry = symtab->entryContainingValue(val);
                if (!revEntry) {
                    qDebug() << entry->name() << entry->value() << entry->size() << entry->type() << entry->bindType();
                }
                QVERIFY(revEntry);
                QVERIFY(val >= revEntry->value());
                QVERIFY(val < revEntry->value() + revEntry->size());
            }
        }
    }
};

QTEST_MAIN(ElfSymbolTableTest)

#include "elfsymboltabletest.moc"
