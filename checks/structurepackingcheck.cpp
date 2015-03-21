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

#include "structurepackingcheck.h"

#include <elf/elffileset.h>
#include <dwarf/dwarfinfo.h>
#include <dwarf/dwarfdie.h>

#include <QBitArray>
#include <QDebug>
#include <QString>
#include <QTextStream>

#include <libdwarf/dwarf.h>

#include <cassert>
#include <iostream>

StructurePackingCheck::~StructurePackingCheck()
{
}

void StructurePackingCheck::setElfFileSet(ElfFileSet* fileSet)
{
    m_fileSet = fileSet;
}

void StructurePackingCheck::checkAll(DwarfInfo* info)
{
    assert(m_fileSet);
    if (!info)
        return;

    for (DwarfDie* die : info->compilationUnits())
        checkDie(die);
}

static QString printSummary(int structSize, int usedBytes, int usedBits, int optimalSize)
{
    QString s;
    s += "Used bytes: " + QString::number(usedBytes) + "/" + QString::number(structSize);
    s += " (" + QString::number(double(usedBytes*100) / double(structSize), 'g', 4) + "%)\n";
    s += "Used bits: " + QString::number(usedBits) + "/" + QString::number(structSize*8);
    s += " (" + QString::number(double(usedBits*100) / double(structSize*8), 'g', 4) + "%)\n";
    if (optimalSize < structSize) {
        s += "Optimal size: " + QString::number(optimalSize) + " bytes (";
        const int saving = structSize - optimalSize;
        s += QString::number(-saving) + " bytes, " + QString::number(double(saving*100) / double(structSize), 'g', 4) + "%)\n";
    }
    return s;
}

QString StructurePackingCheck::checkOneStructure(DwarfDie* structDie) const
{
    assert(structDie->tag() == DW_TAG_class_type || structDie->tag() == DW_TAG_structure_type);

    QVector<DwarfDie*> members;
    for (DwarfDie* child : structDie->children()) {
        if (child->tag() == DW_TAG_member && !child->isStaticMember())
            members.push_back(child);
        else if (child->tag() == DW_TAG_inheritance)
            members.push_back(child);
    }

    const int structSize = structDie->typeSize();
    int usedBytes;
    int usedBits;
    std::tie(usedBytes, usedBits) = computeStructureMemoryUsage(structDie, members);
    const int optimalSize = optimalStructureSize(structDie, members);

    QString s = printSummary(structSize, usedBytes, usedBits, optimalSize);
    s += '\n';
    s += printStructure(structDie, members);
    return s;
}

static QString sourceLocation(DwarfDie *die)
{
    return die->attribute(DW_AT_decl_file).toString() + ':' + QString::number(die->attribute(DW_AT_decl_line).toInt());
}

void StructurePackingCheck::checkDie(DwarfDie* die)
{
    if (die->tag() == DW_TAG_structure_type || die->tag() == DW_TAG_class_type) {
        QVector<DwarfDie*> members;
        for (DwarfDie* child : die->children()) {
            if (child->tag() == DW_TAG_member && !child->isStaticMember())
                members.push_back(child);
            else if (child->tag() == DW_TAG_inheritance)
                members.push_back(child);
            else
                checkDie(child);
        }

        const int structSize = die->typeSize();
        if (structSize <= 0)
            return;

        int usedBytes;
        int usedBits;
        std::tie(usedBytes, usedBits) = computeStructureMemoryUsage(die, members);
        const int optimalSize = optimalStructureSize(die, members);

        if ((usedBytes != structSize || usedBits != structSize * 8) && optimalSize != structSize) {
            const QString loc = sourceLocation(die);
            if (m_duplicateCheck.contains(loc))
                return;
            std::cout << printSummary(structSize, usedBytes, usedBits, optimalSize).toLocal8Bit().constData();
            std::cout << printStructure(die, members).toLocal8Bit().constData();
            std::cout << std::endl;
            m_duplicateCheck.insert(loc);
        }

    } else {
        for (DwarfDie* child : die->children())
            checkDie(child);
    }
}

static int countBytes(const QBitArray &bits)
{
    int count = 0;
    for (int byteIndex = 0; byteIndex < bits.size() / 8; ++byteIndex) {
        for (int bitIndex = 0; bitIndex < 8; ++bitIndex) {
            if (bits[byteIndex * 8 + bitIndex]) {
                ++count;
                break;
            }
        }
    }
    return count;
}

static int countBits(const QBitArray &bits)
{
    int count = 0;
    for (int i = 0; i < bits.size(); ++i) {
        if (bits[i])
            ++count;
    }
    return count;
}

std::tuple<int, int> StructurePackingCheck::computeStructureMemoryUsage(DwarfDie* structDie, const QVector< DwarfDie* >& memberDies) const
{
    const auto structSize = structDie->typeSize();
    if (structSize <= 0)
        return {};

    assert(structSize > 0);
    QBitArray memUsage(structSize * 8);

    for (DwarfDie *memberDie : memberDies) {
        const auto memberTypeDie = findTypeDefinition(memberDie->attribute(DW_AT_type).value<DwarfDie*>());
        assert(memberTypeDie);

        const auto memberLocation = memberDie->attribute(DW_AT_data_member_location).toInt();
        const auto bitSize = memberDie->attribute(DW_AT_bit_size).toInt();
        const auto bitOffset = memberDie->attribute(DW_AT_bit_offset).toInt();

        if (bitSize <= 0) {
            assert((structSize * 8) >= (memberLocation * 8 + memberTypeDie->typeSize() * 8));
            memUsage.fill(true, memberLocation * 8, memberLocation * 8 + memberTypeDie->typeSize() * 8);
        } else {
            assert((structSize * 8) >= (memberLocation * 8 + bitOffset + bitSize));
            memUsage.fill(true, memberLocation * 8 + bitOffset, memberLocation * 8 + bitOffset + bitSize);
        }
    }

    const auto usedBytes = countBytes(memUsage);
    const auto usedBits = countBits(memUsage);
    return std::make_tuple(usedBytes, usedBits);
}

static QByteArray fullyQualifiedName(DwarfDie* structDie)
{
    QByteArray baseName;
    DwarfDie* parentDie = structDie->parentDIE();
    if (parentDie->tag() == DW_TAG_class_type || parentDie->tag() == DW_TAG_structure_type || parentDie->tag() == DW_TAG_namespace)
        baseName = fullyQualifiedName(parentDie) + "::";
    return baseName + structDie->name();
}

static bool hasUnknownSize(DwarfDie *typeDie)
{
    // 0-size elements can exist, see e.g. __flexarr in inotify.h
    return typeDie->typeSize() == 0 && (typeDie->tag() == DW_TAG_class_type || typeDie->tag() == DW_TAG_structure_type);
}

QString StructurePackingCheck::printStructure(DwarfDie* structDie, const QVector<DwarfDie*>& memberDies) const
{
    QString str;
    QTextStream s(&str);

    s << (structDie->tag() == DW_TAG_class_type ? "class " : "struct ");
    s << fullyQualifiedName(structDie);
    s << " // location: " << sourceLocation(structDie);
    s << "\n{\n";

    bool skipPadding = false; // TODO this should not be needed, look outside of the current CU for the real DIE?
    int nextMemberLocation = 0;
    for (DwarfDie *memberDie : memberDies) {
        s << "    ";

        if (memberDie->tag() == DW_TAG_inheritance)
            s << "inherits ";

        DwarfDie *unresolvedTypeDie = memberDie->attribute(DW_AT_type).value<DwarfDie*>();
        const auto memberTypeDie = findTypeDefinition(unresolvedTypeDie);
        assert(memberTypeDie);

        const auto memberLocation = memberDie->attribute(DW_AT_data_member_location).toInt();
        if (memberLocation > nextMemberLocation && !skipPadding) {
            s << "// " << (memberLocation - nextMemberLocation) << " byte(s) padding\n";
            s << "    ";
        }

        // we use the unresolved DIE here to have the user-visible type name, e.g. of a typedef
        s << unresolvedTypeDie->typeName(); // TODO this does not work for unions/structs/classes
        s << " ";
        s << memberDie->name();

        const auto bitSize = memberDie->attribute(DW_AT_bit_size).toInt();
        if (bitSize > 0) {
            s << ':' << bitSize;
        }

        s << "; // member offset: " << memberLocation;

        if (hasUnknownSize(memberTypeDie)) {
            s << ", unknown size";
            skipPadding = true;
        } else {
            s << ", size: " << memberTypeDie->typeSize();
            skipPadding = false;
        }
        s << ", alignment: " << memberTypeDie->typeAlignment();

        if (bitSize > 0) {
            const auto bitOffset = memberDie->attribute(DW_AT_bit_offset).toInt();
            s << ", bit offset: " << bitOffset;
        }

        s << "\n";

        nextMemberLocation = memberLocation + memberTypeDie->typeSize();
    }

    if (nextMemberLocation < structDie->typeSize() && !skipPadding)
        s << "    // " << (structDie->typeSize() - nextMemberLocation) << " byte(s) padding\n";

    s << "}; // size: " << structDie->typeSize();
    s << ", alignment: " << structDie->typeAlignment();
    s << "\n";
    return str;
}

static bool isEmptyBaseClass(DwarfDie* inheritanceDie)
{
    assert(inheritanceDie->tag() == DW_TAG_inheritance);
    const auto baseTypeDie = inheritanceDie->attribute(DW_AT_type).value<DwarfDie*>();
    if (baseTypeDie->typeSize() != 1)
        return false;

    for (DwarfDie *d : baseTypeDie->children()) {
        if (d->tag() == DW_TAG_member)
            return false;
        if (d->tag() != DW_TAG_inheritance)
            continue;
        if (!isEmptyBaseClass(d))
            return false;
    }
    return true;
}

int StructurePackingCheck::optimalStructureSize(DwarfDie* structDie, const QVector< DwarfDie* >& memberDies) const
{
    int size = 0;
    int alignment = 1;
    QVector<int> sizes;

    // TODO: lots of stuff missing to compute optimal bit field layout
    int prevMemberLocation = -1;
    bool guessSize = false; // TODO see above, this probably needs better lookup for external types
    for (DwarfDie* memberDie : memberDies) {
        // consider empty base class optimization, they don't count, unless we are entirely empty, which is handled at the very end
        if (memberDie->tag() == DW_TAG_inheritance && isEmptyBaseClass(memberDie))
            continue;

        if (prevMemberLocation == memberDie->attribute(DW_AT_data_member_location))
            continue; // skip bit fields for now

        const auto memberTypeDie = findTypeDefinition(memberDie->attribute(DW_AT_type).value<DwarfDie*>());
        assert(memberTypeDie);

        const auto memberLocation = memberDie->attribute(DW_AT_data_member_location).toInt();
        if (guessSize)
            sizes.push_back(memberLocation - prevMemberLocation);

        guessSize = hasUnknownSize(memberTypeDie);
        sizes.push_back(memberTypeDie->typeSize());
        alignment = std::max(alignment, memberTypeDie->typeAlignment());

        prevMemberLocation = memberLocation;
    }

    if (guessSize)
        sizes.push_back(structDie->typeSize() - prevMemberLocation);

    // TODO: sort by alignment and add padding
    for (const int s : sizes)
        size += s;

    // align the entire struct to maximum member alignment
    if (size % alignment)
        size += alignment - (size % alignment);

    // structs are always at least 1 byte
    return std::max(1, size);
}

static DwarfDie* findTypeDefinitionRecursive(DwarfDie *die, const QVector<QByteArray> &fullId)
{
    // TODO filter to namespace/class/struct tags?
    if (die->name() != fullId.first())
        return nullptr;
    if (fullId.size() == 1)
        return die;

    QVector<QByteArray> partialId = fullId;
    partialId.pop_front();
    for (DwarfDie* child : die->children()) {
        DwarfDie *found = findTypeDefinitionRecursive(child, partialId);
        if (found)
            return found;
    }
    return nullptr;
}

DwarfDie* StructurePackingCheck::findTypeDefinition(DwarfDie* typeDie) const
{
    // recurse into typedefs
    if (typeDie->tag() == DW_TAG_typedef)
        return findTypeDefinition(typeDie->attribute(DW_AT_type).value<DwarfDie*>());

    if (!hasUnknownSize(typeDie))
        return typeDie;

    // deterine the full identifier of the type
    QVector<QByteArray> fullId;
    DwarfDie *parentDie = typeDie;
    do {
        fullId.prepend(parentDie->name());
        parentDie = parentDie->parentDIE();
    } while (parentDie && parentDie->tag() != DW_TAG_compile_unit);

    // sequential search in all CUs for a DIE with the same full id containing the full definition
    for (int i = 0; i < m_fileSet->size(); ++i) {
        const auto file = m_fileSet->file(i);
        if (!file->dwarfInfo())
            continue;

        for (DwarfDie *cuDie : file->dwarfInfo()->compilationUnits()) {
            for (DwarfDie *topDie : cuDie->children()) {
                DwarfDie *die = findTypeDefinitionRecursive(topDie, fullId);
                if (die && die->typeSize() > 0) {
                    //qDebug() << "replacing" << typeDie->displayName() << "with" << die->displayName();
                    return die;
                }
            }
        }
    }

    // no luck
    qDebug() << "didn't fine a full definition for" << typeDie->displayName();
    return typeDie;
}
