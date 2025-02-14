/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef STRUCTUREPACKINGCHECK_H
#define STRUCTUREPACKINGCHECK_H

#include <QSet>

class ElfFileSet;
class DwarfInfo;
class DwarfDie;

class QString;

class StructurePackingCheck
{
public:
    StructurePackingCheck() = default;
    StructurePackingCheck(const StructurePackingCheck&) = default;
    ~StructurePackingCheck() = default;

    StructurePackingCheck& operator=(const StructurePackingCheck&) = default;

    /** Set the ELF file set the checked DWARF info belongs to.*/
    void setElfFileSet(ElfFileSet *fileSet);

    void checkAll(DwarfInfo* info);
    QString checkOneStructure(DwarfDie *structDie) const;

private:
    void checkDie(DwarfDie* die);
    std::tuple<int, int> computeStructureMemoryUsage(DwarfDie* structDie, const QList<DwarfDie*> &memberDies) const;
    QString printStructure(DwarfDie* structDie, const QList<DwarfDie*>& memberDies) const;
    int optimalStructureSize(DwarfDie* structDie, const QList<DwarfDie*> &memberDies) const;
    /** Look for a better type DIE for the given external one (@p typeDie). */
    DwarfDie* findTypeDefinition(DwarfDie *typeDie) const;

    ElfFileSet *m_fileSet = nullptr;
    QSet<QString> m_duplicateCheck;
};

#endif // STRUCTUREPACKINGCHECK_H
