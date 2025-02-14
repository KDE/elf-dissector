/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "config-elf-dissector.h"
#include "dwarfprinter.h"

#if HAVE_DWARF
#include <dwarf/dwarfexpression.h>
#include <dwarf/dwarfranges.h>
#include <dwarf/dwarfdie.h>

#include <libdwarf.h>
#endif

#include <QMetaType>
#include <QString>

#include <cassert>
#include <type_traits>

using namespace Qt::Literals;

const char* DwarfPrinter::virtuality(DwarfVirtuality v)
{
#if HAVE_DWARF
    const char *str = nullptr;
    const auto res = dwarf_get_VIRTUALITY_name(static_cast<int>(v), &str);
    if (res != DW_DLV_OK)
        return nullptr;
    return str;
#else
    return nullptr;
#endif
}

QByteArray DwarfPrinter::ranges(const DwarfRanges& ranges)
{
#if HAVE_DWARF
    QByteArray b;
    for (int i = 0; i < ranges.size(); ++i) {
        const auto r = ranges.entry(i);
        static_assert(std::is_same< decltype(r->dwr_addr1), Dwarf_Addr >::value, "Incompatible DWARFs");
        static_assert(std::is_same< decltype(r->dwr_addr2), Dwarf_Addr >::value, "Incompatible DWARFs");
        static_assert( std::is_convertible<Dwarf_Addr, qulonglong>::value, "Incompatible DWARFs" );
        if (r->dwr_type == DW_RANGES_ADDRESS_SELECTION) {
            if (r->dwr_addr2 != 0)
                b += "base address: 0x" + QByteArray::number(static_cast<qulonglong>(r->dwr_addr2), 16) + ' ';
        } else {
            b += "[0x" + QByteArray::number(static_cast<qulonglong>(r->dwr_addr1), 16);
            b += ", 0x" + QByteArray::number(static_cast<qulonglong>(r->dwr_addr2), 16);
            b += "]\n";
        }
    }
    return b;
#else
    return {};
#endif
}

void DwarfPrinter::registerConverterFunctions()
{
#if HAVE_DWARF
    QMetaType::registerConverter<DwarfExpression, QString>(&DwarfExpression::displayString);
    QMetaType::registerConverter<DwarfVirtuality, QString>([](DwarfVirtuality v){
        return QString::fromLatin1(DwarfPrinter::virtuality(v));
    });
    QMetaType::registerConverter<DwarfRanges, QString>([](const DwarfRanges &r){
        return QString::fromLatin1(DwarfPrinter::ranges(r));
    });
#endif
}

QString DwarfPrinter::dieRichText(DwarfDie* die)
{
#if HAVE_DWARF
    assert(die);
    QString s;
    s += "TAG: "_L1 + QLatin1String(die->tagName()) + "<br/>"_L1;
    s += "Name: "_L1 + QString::fromUtf8(die->name()).toHtmlEscaped() + "<br/>"_L1;
    s += "Offset: "_L1 + QString::number(die->offset()) + "<br/>"_L1;
    for (const auto attrType : die->attributes()) {
        const QVariant attrValue = die->attribute(attrType);
        QString attrValueStr;
        if (DwarfDie *die = attrValue.value<DwarfDie*>())
            attrValueStr = die->displayName();
        else
            attrValueStr = attrValue.toString();
        s += QLatin1String(die->attributeName(attrType)) + ": "_L1 + attrValueStr.toHtmlEscaped() + "<br/>"_L1;
    }
    return s;
#else
    return {};
#endif
}
