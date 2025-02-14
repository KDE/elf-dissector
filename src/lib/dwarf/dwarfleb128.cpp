/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dwarfleb128.h"

namespace DwarfLEB128 {

// see https://en.wikipedia.org/wiki/LEB128

uint64_t decodeUnsigned(const char* data, int* size)
{
    const auto begin = data;
    uint64_t result = 0;
    unsigned int shift = 0;
    uint8_t currentByte = 0;

    do {
        currentByte = *data++;
        result |= uint64_t(currentByte & 0x7f) << shift;
        shift += 7;
    } while (currentByte & 0x80);

    if (size)
        *size = data - begin;

    return result;
}

int64_t decodeSigned(const char* data, int* size)
{
    const auto begin = data;
    int64_t result = 0;
    unsigned int shift = 0;
    uint8_t currentByte = 0;

    do {
        currentByte = *data++;
        result |= ((currentByte & 0x7f) << shift);
        shift += 7;
    } while (currentByte & 0x80);

    if (currentByte & 0x40)
        result |= uint64_t(-1) << shift;

    if (size)
        *size = data - begin;

    return result;
}

}
