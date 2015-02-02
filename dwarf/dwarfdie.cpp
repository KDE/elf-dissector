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

#include <QString>

#include <libdwarf/dwarf.h>
#include <libdwarf/libdwarf.h>

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
    for (int i = 0; i < m_srcFileCount; ++i) {
        dwarf_dealloc(dwarfHandle(), m_srcFiles[i], DW_DLA_STRING);
    }
    dwarf_dealloc(dwarfHandle(), m_srcFiles, DW_DLA_LIST);
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

QString DwarfDie::name() const
{
    Q_ASSERT(m_die);

    char* dwarfStr;
    const auto res = dwarf_diename(m_die, &dwarfStr, nullptr);
    if (res != DW_DLV_OK)
        return {};
    const QString s = QString::fromLocal8Bit(dwarfStr);
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

const char* DwarfDie::tagName() const
{
    const char* tagName;
    const auto res = dwarf_get_TAG_name(tag(), &tagName);
    if (res != DW_DLV_OK)
        return {};
    return tagName;
}

Dwarf_Off DwarfDie::offset() const
{
    Dwarf_Off offset;
    const auto res = dwarf_dieoffset(m_die, &offset, nullptr);
    return offset;
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

QVector< QPair< QString, QVariant > > DwarfDie::attributes() const
{
    Dwarf_Attribute* attrList;
    Dwarf_Signed attrCount;
    auto res = dwarf_attrlist(m_die, &attrList, &attrCount, nullptr);
    if (res != DW_DLV_OK)
        return {};

    QVector<QPair<QString, QVariant>> attrs;
    for (int i = 0; i < attrCount; ++i) {
        Dwarf_Half attrType;
        res = dwarf_whatattr(attrList[i], &attrType, nullptr);
        if (res != DW_DLV_OK)
            continue;
        const char* attrName;
        res = dwarf_get_AT_name(attrType, &attrName);
        if (res != DW_DLV_OK)
            continue;

        Dwarf_Half formType;
        res = dwarf_whatform(attrList[i], &formType, nullptr);
        if (res != DW_DLV_OK)
            continue;

        QVariant value;
        switch (formType) {
            case DW_FORM_data1:
            case DW_FORM_data2:
            case DW_FORM_data4:
            case DW_FORM_data8:
            case DW_FORM_udata:
            {
                Dwarf_Unsigned n;
                res = dwarf_formudata(attrList[i], &n, nullptr);
                value = n;
                break;
            }
            case DW_FORM_sdata:
            {
                Dwarf_Signed n;
                res = dwarf_formsdata(attrList[i], &n, nullptr);
                value = n;
                break;
            }
            case DW_FORM_string:
            case DW_FORM_strp:
            {
                char *str;
                res = dwarf_formstring(attrList[i], &str, nullptr);
                value = QString::fromLocal8Bit(str);
                break;
            }
            case DW_FORM_flag:
            case DW_FORM_flag_present:
            {
                Dwarf_Bool b;
                res = dwarf_formflag(attrList[i], &b, nullptr);
                value = b;
                break;
            }
            case DW_FORM_ref1:
            case DW_FORM_ref2:
            case DW_FORM_ref4:
            case DW_FORM_ref8:
            {
                Dwarf_Off offset;
                res = dwarf_global_formref(attrList[i], &offset, nullptr);
                const auto refDie = dwarfInfo()->dieAtOffset(offset);
                const QString refName = refDie->name();
                if (!refName.isEmpty())
                    value = refName;
                else
                    value = QString::number(offset) + " (" + refDie->tagName() + ")";
                break;
            }
            default:
            {
                const char* formName;
                res = dwarf_get_FORM_name(formType, &formName);
                if (res != DW_DLV_OK)
                    continue;
                value = QString("TODO: ") + QString::fromLocal8Bit(formName);
                break;
            }
        }

        // post-process some well-known types
        switch (attrType) {
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

        attrs.push_back(qMakePair(QString::fromLocal8Bit(attrName), value));
    }

    dwarf_dealloc(dwarfHandle(), attrList, DW_DLA_LIST);
    return attrs;
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
