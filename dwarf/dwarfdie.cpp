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
            case DW_FORM_string:
            case DW_FORM_strp:
            {
                char *str;
                res = dwarf_formstring(attrList[i], &str, nullptr);
                value = QString::fromLocal8Bit(str);
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

        attrs.push_back(qMakePair(QString::fromLocal8Bit(attrName), value));
    }

    dwarf_dealloc(dwarfHandle(), attrList, DW_DLA_LIST);
    return attrs;
}

Dwarf_Debug DwarfDie::dwarfHandle() const
{
    return dwarfInfo()->dwarfHandle();
}
