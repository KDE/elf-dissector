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
#include <elf/elffileset.h>
#include <elf/elfsymboltablesection.h>
#include <elf/elfgnusymbolversiontable.h>
#include <elf/elfgnusymbolversiondefinitionssection.h>
#include <elf/elfgnusymbolversiondefinition.h>
#include <elf/elfgnusymbolversionrequirementssection.h>
#include <elf/elfgnusymbolversionrequirement.h>
#include <elf/elfgnusymbolversiondefinitionauxiliaryentry.h>

#include <QDebug>
#include <QtTest/qtest.h>
#include <QObject>

#include <elf.h>

class ElfGNUSymbolVersioningTest: public QObject
{
    Q_OBJECT
private slots:
    void testSymbolVersioning()
    {
        ElfFileSet set;
        set.addFile(QStringLiteral(BINDIR "elf-dissector"));
        QVERIFY(set.size() > 1);

        // we need a full library for this, not just an executable
        ElfFile *f = nullptr;
        for (int i = 0; i < set.size(); ++i) {
            if (set.file(i)->dynamicSection()->soName() == "libQt5Core.so.5")
                f = set.file(i);
        }
        QVERIFY(f);

        const auto symVerIndex = f->indexOfSection(".gnu.version");
        QVERIFY(symVerIndex > 0);
        QCOMPARE(symVerIndex, f->indexOfSection(SHT_GNU_versym));
        const auto symbolVersionTable = f->section<ElfGNUSymbolVersionTable>(symVerIndex);
        QVERIFY(symbolVersionTable);
        QCOMPARE(symbolVersionTable->header()->entryCount(), f->section<ElfSymbolTableSection>(f->indexOfSection(".dynsym"))->header()->entryCount());

        const auto symDefIndex = f->indexOfSection(".gnu.version_d");
        QVERIFY(symDefIndex > 0);
        QCOMPARE(symDefIndex, f->indexOfSection(SHT_GNU_verdef));
        const auto symbolVersionDefs = f->section<ElfGNUSymbolVersionDefinitionsSection>(symDefIndex);
        QVERIFY(symbolVersionDefs);

        QCOMPARE(f->dynamicSection()->entryWithTag(DT_VERDEFNUM)->value(), (uint64_t)symbolVersionDefs->entryCount());
        QVERIFY(symbolVersionDefs->entryCount() > 0);

        const auto verDef = symbolVersionDefs->definition(0);
        QVERIFY(verDef);
        QVERIFY(verDef->auxiliarySize() > 0);

        const auto verDefAux = verDef->auxiliaryEntry(0);
        QVERIFY(verDefAux);

        const auto symNeedIndex = f->indexOfSection(".gnu.version_r");
        QVERIFY(symNeedIndex > 0);
        QCOMPARE(symNeedIndex, f->indexOfSection(SHT_GNU_verneed));
        const auto symbolVersionNeeds = f->section<ElfGNUSymbolVersionRequirementsSection>(symNeedIndex);
        QVERIFY(symbolVersionNeeds);

        QCOMPARE(f->dynamicSection()->entryWithTag(DT_VERNEEDNUM)->value(), (uint64_t)symbolVersionNeeds->entryCount());
        QVERIFY(symbolVersionNeeds->entryCount() > 0);

        const auto verNeed = symbolVersionNeeds->requirement(0);
        QVERIFY(verNeed);
        QVERIFY(verNeed->auxiliarySize() > 0);
    }

    void testSymbolVersionDefinitions()
    {
        ElfFileSet set;
        set.addFile(QStringLiteral(BINDIR "libversioned-symbols.so"));
        QVERIFY(set.size() > 1);

        auto f = set.file(0);
        QVERIFY(f);

        const auto symDefIndex = f->indexOfSection(".gnu.version_d");
        const auto symbolVersionDefs = f->section<ElfGNUSymbolVersionDefinitionsSection>(symDefIndex);
        QVERIFY(symbolVersionDefs);
        QCOMPARE(symbolVersionDefs->entryCount(), 3u);

        ElfGNUSymbolVersionDefinition *defV1 = nullptr, *defV2 = nullptr;
        auto def = symbolVersionDefs->definition(1);
        QVERIFY(def);
        QCOMPARE(def->versionIndex(), (uint16_t)2);
        QCOMPARE(symbolVersionDefs->definitionForVersionIndex(2), def);
        if (def->auxiliarySize() == 1)
            defV1 = def;
        else
            defV2 = def;

        def = symbolVersionDefs->definition(2);
        QVERIFY(def);
        QCOMPARE(def->versionIndex(), (uint16_t)3);
        QCOMPARE(symbolVersionDefs->definitionForVersionIndex(3), def);
        if (def->auxiliarySize() == 1)
            defV1 = def;
        else
            defV2 = def;

        QVERIFY(defV1);
        QVERIFY(defV2);

        QCOMPARE(defV2->auxiliarySize(), (uint16_t)2);
        auto defEntry = defV2->auxiliaryEntry(0);
        QVERIFY(defEntry);
        QCOMPARE(defEntry->name(), "VER2");
        defEntry = defV2->auxiliaryEntry(1);
        QVERIFY(defEntry);
        QCOMPARE(defEntry->name(), "VER1");

        QCOMPARE(defV1->auxiliarySize(), (uint16_t)1);
        defEntry = defV1->auxiliaryEntry(0);
        QVERIFY(defEntry);
        QCOMPARE(defEntry->name(), "VER1");

        const auto symVerIndex = f->indexOfSection(".gnu.version");
        const auto symbolVersionTable = f->section<ElfGNUSymbolVersionTable>(symVerIndex);
        QVERIFY(symbolVersionTable);

        const auto dynTabIndex = f->indexOfSection(".dynsym");
        QVERIFY(dynTabIndex > 0);
        const auto symTab = f->section<ElfSymbolTableSection>(dynTabIndex);
        QVERIFY(symTab);
        QCOMPARE(symTab->header()->entryCount(), symbolVersionTable->header()->entryCount());

        ElfSymbolTableEntry *f1 = nullptr, *f2 = nullptr, *f_ver1 = nullptr, *f_ver2 = nullptr;
        for (uint i = 0; i < symTab->header()->entryCount(); ++i) {
            auto sym = symTab->entry(i);
            QVERIFY(sym);
            if (strcmp(sym->name(), "function1") == 0)
                f1 = sym;
            else if (strcmp(sym->name(), "function2") == 0)
                f2 = sym;
            else if (strcmp(sym->name(), "function") == 0) {
                qDebug() << symbolVersionTable->versionIndex(i);
                if (symbolVersionTable->versionIndex(i) == defV2->versionIndex())
                    f_ver2 = sym;
                else if (symbolVersionTable->versionIndex(i) == defV1->versionIndex())
                    f_ver1 = sym;
            }
        }

        QVERIFY(f1);
        QVERIFY(f2);
        QVERIFY(f_ver1);
        QVERIFY(f_ver2);
        QCOMPARE(f1->value(), f_ver1->value());
        QCOMPARE(f2->value(), f_ver2->value());
    }
};

QTEST_MAIN(ElfGNUSymbolVersioningTest)

#include "elfgnusymbolversioningtest.moc"
