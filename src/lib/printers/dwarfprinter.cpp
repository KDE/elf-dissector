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

#include "dwarfprinter.h"

#include <dwarf/dwarfexpression.h>
#include <dwarf/dwarfranges.h>
#include <dwarf/dwarfdie.h>

#include <QMetaType>
#include <QString>

#include <libdwarf.h>
#include <cassert>

const char* DwarfPrinter::virtuality(DwarfVirtuality v)
{
    const char *str = nullptr;
    const auto res = dwarf_get_VIRTUALITY_name(static_cast<int>(v), &str);
    if (res != DW_DLV_OK)
        return nullptr;
    return str;
}

QByteArray DwarfPrinter::ranges(const DwarfRanges& ranges)
{
    QByteArray b;
    for (int i = 0; i < ranges.size(); ++i) {
        const auto r = ranges.entry(i);
        if (r->dwr_type == DW_RANGES_ADDRESS_SELECTION) {
            if (r->dwr_addr2 != 0)
                b += "base address: 0x" + QByteArray::number(r->dwr_addr2, 16) + ' ';
        } else {
            b += "[0x" + QByteArray::number(r->dwr_addr1, 16);
            b += ", 0x" + QByteArray::number(r->dwr_addr2, 16);
            b += "]\n";
        }
    }
    return b;
}

void DwarfPrinter::registerConverterFunctions()
{
    QMetaType::registerConverter<DwarfExpression, QString>(&DwarfExpression::displayString);
    QMetaType::registerConverter<DwarfVirtuality, QString>([](DwarfVirtuality v){
        return QString::fromLatin1(DwarfPrinter::virtuality(v));
    });
    QMetaType::registerConverter<DwarfRanges, QString>([](const DwarfRanges &r){
        return QString::fromLatin1(DwarfPrinter::ranges(r));
    });
}

QString DwarfPrinter::dieRichText(DwarfDie* die)
{
    assert(die);
    QString s;
    s += "TAG: " + QLatin1String(die->tagName()) + "<br/>";
    s += "Name: " + QString::fromUtf8(die->name()).toHtmlEscaped() + "<br/>";
    s += "Offset: " + QString::number(die->offset()) + "<br/>";
    foreach (const auto attrType, die->attributes()) {
        const QVariant attrValue = die->attribute(attrType);
        QString attrValueStr;
        if (DwarfDie *die = attrValue.value<DwarfDie*>())
            attrValueStr = die->displayName();
        else
            attrValueStr = attrValue.toString();
        s += QLatin1String(die->attributeName(attrType)) + ": " + attrValueStr.toHtmlEscaped() + "<br/>";
    }
    return s;
}
