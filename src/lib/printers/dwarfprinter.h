/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DWARFPRINTER_H
#define DWARFPRINTER_H

#include <dwarf/dwarftypes.h>

class DwarfDie;
class DwarfRanges;
class QString;

/** Pretty printers for DWARF types. */
namespace DwarfPrinter
{
    const char* virtuality(DwarfVirtuality v);

    QByteArray ranges(const DwarfRanges &ranges);

    /** Register string converters for QVariant. */
    void registerConverterFunctions();

    /** Detailed rich-text description of the given DIE. */
    QString dieRichText(DwarfDie *die);
}

#endif // DWARFPRINTER_H
