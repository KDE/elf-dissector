/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SYMBOLPRINTER_H
#define SYMBOLPRINTER_H

#include <cstdint>

class QByteArray;

/** Pretty printers for symbol tables and symbol table entries. */
namespace SymbolPrinter
{
    QByteArray bindType(uint8_t type);
    QByteArray symbolType(uint8_t type);
    QByteArray visibility(uint8_t visibility);
}

#endif // SYMBOLPRINTER_H
