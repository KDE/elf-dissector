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

#include <elf/elffile.h>
#include <elf/elfsymboltablesection.h>
#include <elf/elfgnusymbolversiontable.h>

#include <QtTest/qtest.h>
#include <QObject>

#include <elf.h>

class ElfGNUSymbolVersioningTest: public QObject
{
    Q_OBJECT
private slots:
    void testSymbolVersioning()
    {
        ElfFile f(BINDIR "single-executable");
        QCOMPARE(f.isValid(), true);

        const auto symVerIndex = f.indexOfSection(".gnu.version");
        QVERIFY(symVerIndex > 0);
        const auto symbolVersionTable = f.section<ElfGNUSymbolVersionTable>(symVerIndex);
        QVERIFY(symbolVersionTable);
        QCOMPARE(symbolVersionTable->header()->entryCount(), f.section<ElfSymbolTableSection>(f.indexOfSection(".dynsym"))->header()->entryCount());
    }
};

QTEST_MAIN(ElfGNUSymbolVersioningTest)

#include "elfgnusymbolversioningtest.moc"
