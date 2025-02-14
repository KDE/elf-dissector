/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef RELOCATIONPRINTER_H
#define RELOCATIONPRINTER_H

#include <cstdint>

class ElfRelocationEntry;
class QByteArray;

/** Pretty printers for relocation information. */
namespace RelocationPrinter
{
    QByteArray label(ElfRelocationEntry* entry);
    QByteArray label(uint16_t machine, uint32_t type);
    QByteArray description(ElfRelocationEntry* entry);
}

#endif // RELOCATIONPRINTER_H
