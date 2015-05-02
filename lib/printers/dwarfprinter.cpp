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

#include "dwarfprinter.h"

#include <dwarf/dwarfexpression.h>

#include <QMetaType>
#include <QString>

#include <libdwarf/libdwarf.h>

const char* DwarfPrinter::virtuality(DwarfVirtuality v)
{
    const char *str = nullptr;
    const auto res = dwarf_get_VIRTUALITY_name(static_cast<int>(v), &str);
    if (res != DW_DLV_OK)
        return nullptr;
    return str;
}

void DwarfPrinter::registerConverterFunctions()
{
    QMetaType::registerConverter<DwarfExpression, QString>(&DwarfExpression::displayString);
    QMetaType::registerConverter<DwarfVirtuality, QString>([](DwarfVirtuality v){
        return QString::fromLatin1(DwarfPrinter::virtuality(v));
    });
}
