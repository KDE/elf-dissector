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

#include <dwarf/dwarfinfo.h>
#include <dwarf/dwarfdie.h>

#include <QBitArray>
#include <QDebug>
#include <QString>
#include <QTextStream>

#include <libdwarf/dwarf.h>

#include <cassert>

void StructurePackingCheck::checkAll(DwarfInfo* info)
{
    if (!info)
        return;

    for (DwarfDie* die : info->compilationUnits())
        checkDie(die);
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
        checkStructure(die, members);
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

static QString sourceLocation(DwarfDie *die)
{
    return die->attribute(DW_AT_decl_file).toString() + ':' + QString::number(die->attribute(DW_AT_decl_line).toInt());
}

void StructurePackingCheck::checkStructure(DwarfDie* structDie, const QVector< DwarfDie* >& memberDies)
{
    const auto structSize = structDie->typeSize();
    if (structSize <= 0)
        return;

    assert(structSize > 0);
    QBitArray memUsage(structSize * 8);

    for (DwarfDie *memberDie : memberDies) {
        const auto memberTypeDie = memberDie->attribute(DW_AT_type).value<DwarfDie*>();
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
    const auto optimalSize = optimalStructureSize(structDie, memberDies);

    if ((usedBytes != structSize || usedBits != structSize * 8) && optimalSize != structSize) {
        const QString loc = sourceLocation(structDie);
        if (m_duplicateCheck.contains(loc))
            return;
        qDebug() << "Struct" << structDie->displayName() << " is sub-optimally packed: " << usedBytes << "/" << structSize << ", " << usedBits << "/" << (structSize * 8);
        qDebug() << "optimal size is: " << optimalSize;
        qDebug() << printStructure(structDie, memberDies);
        m_duplicateCheck.insert(loc);
    }
}

static QString fullyQualifiedName(DwarfDie* structDie)
{
    QString baseName;
    DwarfDie* parentDie = structDie->parentDIE();
    if (parentDie->tag() == DW_TAG_class_type || parentDie->tag() == DW_TAG_structure_type || parentDie->tag() == DW_TAG_namespace)
        baseName = fullyQualifiedName(parentDie) + "::";
    return baseName + structDie->name();
}

QString StructurePackingCheck::printStructure(DwarfDie* structDie, const QVector<DwarfDie*>& memberDies)
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

        const auto memberTypeDie = memberDie->attribute(DW_AT_type).value<DwarfDie*>();
        assert(memberTypeDie);

        const auto memberLocation = memberDie->attribute(DW_AT_data_member_location).toInt();
        if (memberLocation > nextMemberLocation && !skipPadding) {
            s << "// " << (memberLocation - nextMemberLocation) << " byte(s) padding\n";
            s << "    ";
        }

        s << memberTypeDie->typeName(); // TODO this does not work for unions/structs/classes
        s << " ";
        s << memberDie->name();

        const auto bitSize = memberDie->attribute(DW_AT_bit_size).toInt();
        if (bitSize > 0) {
            s << ':' << bitSize;
        }

        s << "; // member offset: " << memberLocation;

        if (memberTypeDie->typeSize() == 0) {
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

int StructurePackingCheck::optimalStructureSize(DwarfDie* structDie, const QVector< DwarfDie* >& memberDies)
{
    int size = 0;
    int alignment = 1;
    QVector<int> sizes;

    // TODO: lots of stuff missing to compute optimal bit field layout
    int prevMemberLocation = -1;
    bool guessSize = false; // TODO see above, this probably needs better lookup for external types
    for (DwarfDie* memberDie : memberDies) {
        if (prevMemberLocation == memberDie->attribute(DW_AT_data_member_location))
            continue; // skip bit fields for now

        const auto memberTypeDie = memberDie->attribute(DW_AT_type).value<DwarfDie*>();
        assert(memberTypeDie);

        const auto memberLocation = memberDie->attribute(DW_AT_data_member_location).toInt();
        if (guessSize)
            sizes.push_back(memberLocation - prevMemberLocation);

        guessSize = memberTypeDie->typeSize() == 0;
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
