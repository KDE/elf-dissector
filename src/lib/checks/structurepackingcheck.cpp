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

#include "config-elf-dissector.h"
#include "structurepackingcheck.h"

#include <elf/elffileset.h>
#if HAVE_DWARF
#include <dwarf/dwarfinfo.h>
#include <dwarf/dwarfdie.h>
#include <dwarf/dwarfcudie.h>
#include <dwarf/dwarfexpression.h>

#include <dwarf.h>
#endif

#include <QBitArray>
#include <QDebug>
#include <QString>
#include <QTextStream>

#include <cassert>
#include <iostream>

using namespace Qt::Literals;

void StructurePackingCheck::setElfFileSet(ElfFileSet* fileSet)
{
    m_fileSet = fileSet;
}

void StructurePackingCheck::checkAll(DwarfInfo* info)
{
#if HAVE_DWARF
    assert(m_fileSet);
    if (!info)
        return;

    for (auto die : info->compilationUnits())
        checkDie(die);
#endif
}

static QString printSummary(int structSize, int usedBytes, int usedBits, int optimalSize)
{
    QString s;
    s += QLatin1String("Used bytes: ") + QString::number(usedBytes) + QLatin1Char('/') + QString::number(structSize);
    s += " ("_L1 + QString::number(double(usedBytes*100) / double(structSize), 'g', 4) + "%)\n"_L1;
    s += QLatin1String("Used bits: ") + QString::number(usedBits) + QLatin1Char('/') + QString::number(structSize*8);
    s += " ("_L1 + QString::number(double(usedBits*100) / double(structSize*8), 'g', 4) + "%)\n"_L1;
    if (optimalSize < structSize) {
        s += "Optimal size: "_L1 + QString::number(optimalSize) + " bytes ("_L1;
        const int saving = structSize - optimalSize;
        s += QString::number(-saving) + " bytes, "_L1 + QString::number(double(saving*100) / double(structSize), 'g', 4) + "%)\n"_L1;
    }
    return s;
}

#if HAVE_DWARF
static int dataMemberLocation(DwarfDie *die)
{
    const auto attr = die->attribute(DW_AT_data_member_location);
    if (attr.canConvert(QVariant::Int))
        return attr.toInt();
    qWarning() << "Cannot convert location of" << die->displayName() << ":" << attr.value<DwarfExpression>().displayString();
    return 0;
}

static bool compareMemberDiesByLocation(DwarfDie *lhs, DwarfDie *rhs)
{
    const auto lhsLoc = dataMemberLocation(lhs);
    const auto rhsLoc = dataMemberLocation(rhs);
    if (lhsLoc == rhsLoc) {
        return lhs->attribute(DW_AT_bit_offset) > rhs->attribute(DW_AT_bit_offset);
    }
    return lhsLoc < rhsLoc;
}
#endif

QString StructurePackingCheck::checkOneStructure(DwarfDie* structDie) const
{
#if HAVE_DWARF
    assert(structDie->tag() == DW_TAG_class_type || structDie->tag() == DW_TAG_structure_type);

    QList<DwarfDie*> members;
    for (auto child : structDie->children()) {
        if (child->tag() == DW_TAG_member && !child->isStaticMember())
            members.push_back(child);
        else if (child->tag() == DW_TAG_inheritance)
            members.push_back(child);
    }
    std::sort(members.begin(), members.end(), compareMemberDiesByLocation);

    const int structSize = structDie->typeSize();
    int usedBytes;
    int usedBits;
    std::tie(usedBytes, usedBits) = computeStructureMemoryUsage(structDie, members);
    const int optimalSize = optimalStructureSize(structDie, members);

    QString s = printSummary(structSize, usedBytes, usedBits, optimalSize);
    s += '\n'_L1;
    s += printStructure(structDie, members);
    return s;
#else
    return {};
#endif
}

void StructurePackingCheck::checkDie(DwarfDie* die)
{
#if HAVE_DWARF
    if (die->tag() == DW_TAG_structure_type || die->tag() == DW_TAG_class_type) {
        QList<DwarfDie*> members;
        for (auto child : die->children()) {
            if (child->tag() == DW_TAG_member && !child->isStaticMember())
                members.push_back(child);
            else if (child->tag() == DW_TAG_inheritance)
                members.push_back(child);
            else
                checkDie(child);
        }
        std::sort(members.begin(), members.end(), compareMemberDiesByLocation);

        const int structSize = die->typeSize();
        if (structSize <= 0)
            return;

        int usedBytes;
        int usedBits;
        std::tie(usedBytes, usedBits) = computeStructureMemoryUsage(die, members);
        const int optimalSize = optimalStructureSize(die, members);

        if ((usedBytes != structSize || usedBits != structSize * 8) && optimalSize != structSize) {
            const QString loc = die->sourceLocation();
            if (m_duplicateCheck.contains(loc))
                return;
            std::cout << printSummary(structSize, usedBytes, usedBits, optimalSize).toLocal8Bit().constData();
            std::cout << printStructure(die, members).toLocal8Bit().constData();
            std::cout << std::endl;
            m_duplicateCheck.insert(loc);
        }

    } else {
        for (auto child : die->children())
            checkDie(child);
    }
#endif
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

#if HAVE_DWARF
static int bitsForEnum(DwarfDie *die)
{
    assert(die->tag() == DW_TAG_enumeration_type);

    // approach 1: count all covered bits
    QBitArray bits(die->typeSize() * 8, false);
    // approach 2: count number of enum values
    int enumCount = 0;

    for (auto child : die->children()) {
        if (child->tag() != DW_TAG_enumerator)
            continue;
        ++enumCount;
        const auto enumValue = child->attribute(DW_AT_const_value).toInt();
        for (int i = 0; i < bits.size(); ++i) {
            if ((1 << i) & enumValue)
                bits[i] = true;
        }
    }
    if (enumCount == 0) {
        return die->typeSize() * 8; // incomplete information or something went wrong here...
    }

    // minimum of the above is our best guess
    return std::min(enumCount - 1, countBits(bits));
}

static int actualTypeSize(DwarfDie *die)
{
    switch (die->tag()) {
        case DW_TAG_base_type:
            if (die->name() == "bool")
                return 1;
            return die->typeSize() * 8;
        case DW_TAG_enumeration_type:
            return bitsForEnum(die);
        case DW_TAG_pointer_type:
            return die->typeSize() * 8; // TODO pointer alignment can save a few bits
        case DW_TAG_const_type:
        case DW_TAG_restrict_type:
        case DW_TAG_typedef:
        case DW_TAG_volatile_type:
        {
            const auto typeDie = die->attribute(DW_AT_type).value<DwarfDie*>();
            assert(typeDie);
            return actualTypeSize(typeDie);
        }
        case DW_TAG_array_type:
        {
            return die->typeSize() * 8; // TODO the below is correct, but we need to distribute that over the memory bit array below, otherwise usedBytes is wrong
/*            const auto typeDie = die->attribute(DW_AT_type).value<DwarfDie*>();
            assert(typeDie);
            return die->typeSize() / typeDie->typeSize() * actualTypeSize(typeDie);*/
        }
    }
    return die->typeSize() * 8;
}
#endif

std::tuple<int, int> StructurePackingCheck::computeStructureMemoryUsage(DwarfDie* structDie, const QList< DwarfDie* >& memberDies) const
{
#if HAVE_DWARF
    const auto structSize = structDie->typeSize();
    if (structSize <= 0)
        return {};

    assert(structSize > 0);
    QBitArray memUsage(structSize * 8);

    for (DwarfDie *memberDie : memberDies) {
        const auto memberTypeDie = findTypeDefinition(memberDie->attribute(DW_AT_type).value<DwarfDie*>());
        assert(memberTypeDie);

        const auto memberLocation = dataMemberLocation(memberDie);
        const auto bitSize = memberDie->attribute(DW_AT_bit_size).toInt();
        const auto bitOffset = memberDie->attribute(DW_AT_bit_offset).toInt();

        if (bitSize <= 0) {
            assert((structSize * 8) >= (memberLocation * 8 + memberTypeDie->typeSize() * 8));
            memUsage.fill(true, memberLocation * 8, memberLocation * 8 + actualTypeSize(memberTypeDie));
        } else {
            assert((structSize * 8) >= (memberLocation * 8 + bitOffset + bitSize));
            memUsage.fill(true, memberLocation * 8 + bitOffset, memberLocation * 8 + bitOffset + bitSize);
        }
    }

    const auto usedBytes = countBytes(memUsage);
    const auto usedBits = countBits(memUsage);
    return std::make_tuple(usedBytes, usedBits);
#else
    return std::make_tuple(0, 0);
#endif
}

#if HAVE_DWARF
static bool hasUnknownSize(DwarfDie *typeDie)
{
    // 0-size elements can exist, see e.g. __flexarr in inotify.h
    return typeDie->typeSize() == 0 && (typeDie->tag() == DW_TAG_class_type || typeDie->tag() == DW_TAG_structure_type);
}
#endif

QString StructurePackingCheck::printStructure(DwarfDie* structDie, const QList<DwarfDie*>& memberDies) const
{
    QString str;
#if HAVE_DWARF
    QTextStream s(&str);

    s << (structDie->tag() == DW_TAG_class_type ? "class " : "struct ");
    s << structDie->fullyQualifiedName();
    s << " // location: " << structDie->sourceLocation();
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

        const auto memberLocation = dataMemberLocation(memberDie);
        if (memberLocation > nextMemberLocation && !skipPadding) {
            s << "// " << (memberLocation - nextMemberLocation) << " byte(s) padding\n";
            s << "    ";
        }

        // we use the unresolved DIE here to have the user-visible type name, e.g. of a typedef
        s << unresolvedTypeDie->fullyQualifiedName();
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

            const auto actualSize = actualTypeSize(memberTypeDie);
            if (actualSize != memberTypeDie->typeSize() * 8)
                s << " (needed: " << actualSize << " bits)";
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
#endif
    return str;
}

static bool isEmptyBaseClass(DwarfDie* inheritanceDie)
{
#if HAVE_DWARF
    assert(inheritanceDie->tag() == DW_TAG_inheritance);
    const auto baseTypeDie = inheritanceDie->attribute(DW_AT_type).value<DwarfDie*>();
    if (baseTypeDie->typeSize() != 1)
        return false;

    for (auto d : baseTypeDie->children()) {
        if (d->tag() == DW_TAG_member)
            return false;
        if (d->tag() != DW_TAG_inheritance)
            continue;
        if (!isEmptyBaseClass(d))
            return false;
    }
#endif
    return true;
}

int StructurePackingCheck::optimalStructureSize(DwarfDie* structDie, const QList< DwarfDie* >& memberDies) const
{
    int size = 0;
#if HAVE_DWARF
    int alignment = 1;
    QList<int> sizes;

    // TODO: lots of stuff missing to compute optimal bit field layout
    int prevMemberLocation = -1;
    bool guessSize = false; // TODO see above, this probably needs better lookup for external types
    for (DwarfDie* memberDie : memberDies) {
        // consider empty base class optimization, they don't count, unless we are entirely empty, which is handled at the very end
        if (memberDie->tag() == DW_TAG_inheritance && isEmptyBaseClass(memberDie))
            continue;

        if (prevMemberLocation == dataMemberLocation(memberDie))
            continue; // skip bit fields for now

        const auto memberTypeDie = findTypeDefinition(memberDie->attribute(DW_AT_type).value<DwarfDie*>());
        assert(memberTypeDie);

        const auto memberLocation = dataMemberLocation(memberDie);
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
    for (const auto s : sizes)
        size += s;

    // align the entire struct to maximum member alignment
    if (size % alignment)
        size += alignment - (size % alignment);
#endif

    // structs are always at least 1 byte
    return std::max(1, size);
}

static DwarfDie* findTypeDefinitionRecursive(DwarfDie *die, const QList<QByteArray> &fullId)
{
#if HAVE_DWARF
    // TODO filter to namespace/class/struct tags?
    if (die->name() != fullId.first())
        return nullptr;
    if (fullId.size() == 1)
        return die;

    QList<QByteArray> partialId = fullId;
    partialId.pop_front();
    for (auto child : die->children()) {
        DwarfDie *found = findTypeDefinitionRecursive(child, partialId);
        if (found)
            return found;
    }
#endif
    return nullptr;
}

DwarfDie* StructurePackingCheck::findTypeDefinition(DwarfDie* typeDie) const
{
#if HAVE_DWARF
    // recurse into typedefs
    if (typeDie->tag() == DW_TAG_typedef)
        return findTypeDefinition(typeDie->attribute(DW_AT_type).value<DwarfDie*>());

    if (!hasUnknownSize(typeDie))
        return typeDie;

    // determine the full identifier of the type
    QList<QByteArray> fullId;
    DwarfDie *parentDie = typeDie;
    do {
        fullId.prepend(parentDie->name());
        parentDie = parentDie->parentDie();
    } while (parentDie && parentDie->tag() != DW_TAG_compile_unit);

    // sequential search in all CUs for a DIE with the same full id containing the full definition
    for (int i = 0; i < m_fileSet->size(); ++i) {
        const auto file = m_fileSet->file(i);
        if (!file->dwarfInfo())
            continue;

        for (auto cuDie : file->dwarfInfo()->compilationUnits()) {
            for (auto topDie : cuDie->children()) {
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
#else
    return nullptr;
#endif
}
