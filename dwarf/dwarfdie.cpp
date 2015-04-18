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

#include "dwarfdie.h"
#include "dwarfinfo.h"

#include <QDebug>
#include <QFileInfo>
#include <QString>

#include <libdwarf/dwarf.h>
#include <libdwarf/libdwarf.h>

#include <cassert>

DwarfDie::DwarfDie(Dwarf_Die die, DwarfDie* parent) :
    m_die(die),
    m_parent(parent)
{
}

DwarfDie::DwarfDie(Dwarf_Die die, DwarfInfo* info) :
    m_die(die),
    m_info(info)
{
}

DwarfDie::~DwarfDie()
{
    qDeleteAll(m_children);

    for (int i = 0; i < m_srcFileCount; ++i) {
        dwarf_dealloc(dwarfHandle(), m_srcFiles[i], DW_DLA_STRING);
    }
    dwarf_dealloc(dwarfHandle(), m_srcFiles, DW_DLA_LIST);

    dwarf_dealloc(dwarfHandle(), m_die, DW_DLA_DIE);
}

DwarfInfo* DwarfDie::dwarfInfo() const
{
    if (m_info)
        return m_info;
    Q_ASSERT(m_parent);
    return parentDIE()->dwarfInfo();
}

DwarfDie* DwarfDie::parentDIE() const
{
    return m_parent;
}

QByteArray DwarfDie::name() const
{
    Q_ASSERT(m_die);

    char* dwarfStr;
    const auto res = dwarf_diename(m_die, &dwarfStr, nullptr);
    if (res != DW_DLV_OK)
        return {};
    const QByteArray s(dwarfStr);
    dwarf_dealloc(dwarfHandle(), dwarfStr, DW_DLA_STRING);
    return s;
}

Dwarf_Half DwarfDie::tag() const
{
    Dwarf_Half tagType;
    const auto res = dwarf_tag(m_die, &tagType, nullptr);
    if (res != DW_DLV_OK)
        return {};
    return tagType;
}

QByteArray DwarfDie::tagName() const
{
    const char* tagName;
    const auto res = dwarf_get_TAG_name(tag(), &tagName);
    if (res != DW_DLV_OK)
        return QByteArray("Unknown tag ") + QByteArray::number(tag());
    return QByteArray::fromRawData(tagName, strlen(tagName));
}

Dwarf_Off DwarfDie::offset() const
{
    Dwarf_Off offset;
    const auto res = dwarf_dieoffset(m_die, &offset, nullptr);
    assert(res == DW_DLV_OK);
    return offset;
}

static QVector<int> arrayDimensions(const DwarfDie *die)
{
    QVector<int> dims;
    for (const auto *child : die->children()) {
        if (child->tag() != DW_TAG_subrange_type)
            continue;
        const auto attr = child->attribute(DW_AT_upper_bound);
        if (attr.isNull())
            dims.push_back(0);
        // DW_AT_upper_bound is the highest allowed index, not the size
        dims.push_back(attr.toInt() + 1);
    }
    return dims;
}

static QByteArrayList argumentList(const DwarfDie *die)
{
    QByteArrayList args;
    for (const auto *child : die->children()) {
        if (child->tag() == DW_TAG_formal_parameter) {
            args.push_back(child->typeName());
        }
    }
    return args;
}

QByteArray DwarfDie::typeName() const
{
    const auto n = name();
    if (!n.isEmpty())
        return n;

    const auto typeDie = attribute(DW_AT_type).value<DwarfDie*>();
    QByteArray typeName;
    if (!typeDie) {
        switch (tag()) {
            case DW_TAG_class_type:
                return "<anon class>";
            case DW_TAG_enumeration_type:
                return "<anon enum>";
            case DW_TAG_structure_type:
                return "<anon struct>";
            case DW_TAG_union_type:
                return "<anon union>";
            case DW_TAG_array_type:
            case DW_TAG_base_type:
            case DW_TAG_const_type:
            case DW_TAG_pointer_type:
            case DW_TAG_ptr_to_member_type:
            case DW_TAG_reference_type:
            case DW_TAG_restrict_type:
            case DW_TAG_rvalue_reference_type:
            case DW_TAG_subroutine_type:
            case DW_TAG_typedef:
                typeName = "void";
                break;
            default:
                return {};
        }
    } else {
        typeName = typeDie->typeName();
    }

    // TODO: function pointers and pointer to members
    switch (tag()) {
        case DW_TAG_pointer_type:
            return typeName + "*";
        case DW_TAG_reference_type:
            return typeName + "&";
        case DW_TAG_rvalue_reference_type:
            return typeName + "&&";
        case DW_TAG_const_type:
            return typeName + " const";
        case DW_TAG_array_type:
        {
            const auto dims = arrayDimensions(this);
            QByteArray n = typeName;
            for (int d : dims)
                n += "[" + QString::number(d) + "]";
            return n;
        }
        case DW_TAG_restrict_type:
            return typeName + " restrcit";
        case DW_TAG_volatile_type:
            return typeName + " volatile";
        case DW_TAG_subroutine_type:
            return typeName + " (*)(" + argumentList(this).join(", ") + ")";
        case DW_TAG_ptr_to_member_type:
        {
            const auto classDie = attribute(DW_AT_containing_type).value<DwarfDie*>();
            QByteArray className;
            if (classDie)
                className = classDie->typeName();
            return typeName + " (" + className + "::*)(" + argumentList(this).join(", ") + ")";
        }
    }
    return typeName;
}

int DwarfDie::typeSize() const
{
    switch (tag()) {
        case DW_TAG_base_type:
        case DW_TAG_class_type:
        case DW_TAG_enumeration_type:
        case DW_TAG_structure_type:
        case DW_TAG_union_type:
            return attribute(DW_AT_byte_size).toInt();
        case DW_TAG_pointer_type:
        case DW_TAG_reference_type:
        case DW_TAG_rvalue_reference_type:
            return 8; // TODO: support 32bit!
        case DW_TAG_const_type:
        case DW_TAG_restrict_type:
        case DW_TAG_typedef:
        case DW_TAG_volatile_type:
        {
            const auto typeDie = attribute(DW_AT_type).value<DwarfDie*>();
            assert(typeDie);
            return typeDie->typeSize();
        }
        case DW_TAG_array_type:
        {
            const auto typeDie = attribute(DW_AT_type).value<DwarfDie*>();
            assert(typeDie);
            int s = typeDie->typeSize();
            for (int d : arrayDimensions(this))
                s *= d;
            return s;
        }
    }

    return 0;
}

int DwarfDie::typeAlignment() const
{
    switch (tag()) {
        case DW_TAG_base_type:
        case DW_TAG_enumeration_type:
            return std::min(typeSize(), 8); // TODO: 32bit support
        case DW_TAG_array_type:
        case DW_TAG_const_type:
        case DW_TAG_restrict_type:
        case DW_TAG_typedef:
        case DW_TAG_volatile_type:
        {
            const auto typeDie = attribute(DW_AT_type).value<DwarfDie*>();
            assert(typeDie);
            return typeDie->typeAlignment();
        }
        case DW_TAG_pointer_type:
        case DW_TAG_reference_type:
        case DW_TAG_rvalue_reference_type:
            return 8; // TODO: 32bit support
        case DW_TAG_class_type:
        case DW_TAG_structure_type:
        case DW_TAG_union_type:
        {
            int align = 1;
            for (const DwarfDie* child : children()) {
                if (child->tag() != DW_TAG_member && child->tag() != DW_TAG_inheritance)
                    continue;
                if (child->isStaticMember())
                    continue;
                const auto typeDie = child->attribute(DW_AT_type).value<DwarfDie*>();
                assert(typeDie);
                align = std::max(align, typeDie->typeAlignment());
            }
            return align;
        }
    }

    return 0;
}

bool DwarfDie::isStaticMember() const
{
    // TODO not entirely sure yet this is correct...
    const auto memberLocationAttr = attribute(DW_AT_data_member_location);
    if (!memberLocationAttr.isNull())
        return false;

    const auto externalAttr = attribute(DW_AT_external).toBool();
    const auto declAttr = attribute(DW_AT_declaration).toBool();
    return externalAttr || declAttr;
}

QString DwarfDie::displayName() const
{
    QString n = name();

    if (n.isEmpty())
        n = typeName();

    if (n.isEmpty()) {
        n = tagName();
        n += " (offset ";
    } else {
        n += " (";
        n += tagName();
        n += ", offset ";
    }
    n += QString::number(offset());
    n += ")";
    return n;
}

QByteArray DwarfDie::fullyQualifiedName() const
{
    QByteArray baseName;
    auto parentDie = parentDIE();
    if (parentDie->tag() == DW_TAG_class_type || parentDie->tag() == DW_TAG_structure_type || parentDie->tag() == DW_TAG_namespace)
        baseName = parentDie->fullyQualifiedName() + "::";
    return baseName + typeName();
}

QString DwarfDie::sourceLocation() const
{
    auto filePath = attribute(DW_AT_decl_file).toString();
    if (filePath.isEmpty())
        return filePath;
    QFileInfo fi(filePath);
    if (fi.isRelative()) {
        QString cuPath;
        DwarfDie const* parentDie = this;
        while (parentDie && parentDie->tag() != DW_TAG_compile_unit)
            parentDie = parentDie->parentDIE();
        if (parentDie)
            fi.setFile(parentDie->name() + "/" + filePath);
    }
    if (fi.exists())
        filePath = fi.canonicalFilePath();

    return  filePath + ':' + QString::number(attribute(DW_AT_decl_line).toInt());
}

static void stringifyEnum(QVariant &value, int (*get_name)(unsigned int, const char**))
{
    const auto i = value.value<Dwarf_Unsigned>();
    const char* str;
    const auto res = get_name(i, &str);
    if (res != DW_DLV_OK)
        return;
    value = QString::fromLocal8Bit(str);
}

QVector< Dwarf_Half > DwarfDie::attributes() const
{
    Dwarf_Attribute* attrList;
    Dwarf_Signed attrCount;
    auto res = dwarf_attrlist(m_die, &attrList, &attrCount, nullptr);
    if (res != DW_DLV_OK)
        return {};

    QVector<Dwarf_Half> attrs;
    attrs.reserve(attrCount);
    for (int i = 0; i < attrCount; ++i) {
        Dwarf_Half attrType;
        res = dwarf_whatattr(attrList[i], &attrType, nullptr);
        if (res != DW_DLV_OK)
            continue;
        attrs.push_back(attrType);
    }

    dwarf_dealloc(dwarfHandle(), attrList, DW_DLA_LIST);
    return attrs;
}

QByteArray DwarfDie::attributeName(Dwarf_Half attributeType) const
{
    const char* attrName;
    const auto res = dwarf_get_AT_name(attributeType, &attrName);
    if (res != DW_DLV_OK)
        return QByteArray("Unknown attribute ") + QByteArray::number(attributeType);
    return QByteArray::fromRawData(attrName, strlen(attrName));
}

QVariant DwarfDie::attribute(Dwarf_Half attributeType) const
{
    Dwarf_Attribute attr;
    auto res = dwarf_attr(m_die, attributeType, &attr, nullptr);
    if (res != DW_DLV_OK)
        return {};

    Dwarf_Half formType;
    res = dwarf_whatform(attr, &formType, nullptr);
    if (res != DW_DLV_OK)
        return {};

    QVariant value;
    switch (formType) {
        case DW_FORM_data1:
        case DW_FORM_data2:
        case DW_FORM_data4:
        case DW_FORM_data8:
        case DW_FORM_udata:
        {
            Dwarf_Unsigned n;
            res = dwarf_formudata(attr, &n, nullptr);
            value = n;
            break;
        }
        case DW_FORM_sdata:
        {
            Dwarf_Signed n;
            res = dwarf_formsdata(attr, &n, nullptr);
            value = n;
            break;
        }
        case DW_FORM_string:
        case DW_FORM_strp:
        {
            char *str;
            res = dwarf_formstring(attr, &str, nullptr);
            value = QByteArray(str);
            break;
        }
        case DW_FORM_flag:
        case DW_FORM_flag_present:
        {
            Dwarf_Bool b;
            res = dwarf_formflag(attr, &b, nullptr);
            value = b;
            break;
        }
        case DW_FORM_ref1:
        case DW_FORM_ref2:
        case DW_FORM_ref4:
        case DW_FORM_ref8:
        {
            Dwarf_Off offset;
            res = dwarf_global_formref(attr, &offset, nullptr);
            value = QVariant::fromValue(dwarfInfo()->dieAtOffset(offset));
            break;
        }
        case DW_FORM_sec_offset:
        {
            Dwarf_Off offset;
            res = dwarf_global_formref(attr, &offset, nullptr);
            value = offset;
            break;
        }
        case DW_FORM_addr:
        {
            Dwarf_Addr addr;
            res = dwarf_formaddr(attr, &addr, nullptr);
            value = addr;
            break;
        }
        default:
        {
            const char* formName;
            res = dwarf_get_FORM_name(formType, &formName);
            if (res != DW_DLV_OK)
                return {};
            value = QString("TODO: ") + QString::fromLocal8Bit(formName);
            break;
        }
    }

    // post-process some well-known types
    switch (attributeType) {
        case DW_AT_decl_file:
        case DW_AT_call_file:
        {
            const auto fileIndex = value.value<Dwarf_Unsigned>();
            // index 0 means not present, TODO handle that
            value = sourceFileForIndex(fileIndex -1);
            break;
        }
        case DW_AT_accessibility:
            stringifyEnum(value, &dwarf_get_ACCESS_name);
            break;
        case DW_AT_language:
            stringifyEnum(value, &dwarf_get_LANG_name);
            break;
        case DW_AT_virtuality:
            stringifyEnum(value, &dwarf_get_VIRTUALITY_name);
            break;
        case DW_AT_visibility:
            stringifyEnum(value, &dwarf_get_VIS_name);
            break;
        case DW_AT_identifier_case:
            stringifyEnum(value, &dwarf_get_ID_name);
            break;
        case DW_AT_inline:
            stringifyEnum(value, &dwarf_get_INL_name);
            break;
        case DW_AT_encoding:
            stringifyEnum(value, &dwarf_get_ATE_name);
            break;
        case DW_AT_ordering:
            stringifyEnum(value, &dwarf_get_ORD_name);
            break;
        case DW_AT_calling_convention:
            stringifyEnum(value, &dwarf_get_CC_name);
            break;
        case DW_AT_discr_list:
            stringifyEnum(value, &dwarf_get_DSC_name);
            break;
        default:
            break;
    }

    return value;
}

QVector< DwarfDie* > DwarfDie::children() const
{
    if (!m_childrenScanned)
        scanChildren();
    return m_children;
}

DwarfDie* DwarfDie::dieAtOffset(Dwarf_Off offset) const
{
    const auto cus = children();
    auto it = std::lower_bound(cus.begin(), cus.end(), offset, [](DwarfDie* lhs, Dwarf_Off rhs) { return lhs->offset() < rhs; });

    if (it != cus.end() && (*it)->offset() == offset)
        return *it;

    Q_ASSERT(it != cus.begin());
    --it;
    return (*it)->dieAtOffset(offset);
}

void DwarfDie::scanChildren() const
{
    m_childrenScanned = true;

    Dwarf_Die childDie;
    auto res = dwarf_child(m_die, &childDie, nullptr);
    if (res != DW_DLV_OK)
        return;

    forever {
        m_children.push_back(new DwarfDie(childDie, const_cast<DwarfDie*>(this)));

        Dwarf_Die siblingDie;
        res = dwarf_siblingof(dwarfHandle(), childDie, &siblingDie, nullptr);
        if (res != DW_DLV_OK)
            return;

        childDie = siblingDie;
    }
}

Dwarf_Debug DwarfDie::dwarfHandle() const
{
    return dwarfInfo()->dwarfHandle();
}

const char* DwarfDie::sourceFileForIndex(int sourceIndex) const
{
    const auto tagType = tag();
    if (tagType != DW_TAG_compile_unit && tagType != DW_TAG_partial_unit && tagType != DW_TAG_type_unit) {
        if (parentDIE())
            return parentDIE()->sourceFileForIndex(sourceIndex);
        return nullptr;
    }

    if (!m_srcFiles) {
        auto res = dwarf_srcfiles(m_die, &m_srcFiles, &m_srcFileCount, nullptr);
        if (res != DW_DLV_OK)
            return nullptr;
    }

    Q_ASSERT(sourceIndex >= 0);
    Q_ASSERT(sourceIndex < m_srcFileCount);
    return m_srcFiles[sourceIndex];
}
