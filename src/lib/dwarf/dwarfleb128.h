/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DWARFLEB128_H
#define DWARFLEB128_H

#include <cstdint>

/** LEB128 decoder. */
namespace DwarfLEB128
{
    uint64_t decodeUnsigned(const char *data, int *size = nullptr);
    int64_t decodeSigned(const char *data, int *size = nullptr);
}

#endif // DWARFLEB128_H
