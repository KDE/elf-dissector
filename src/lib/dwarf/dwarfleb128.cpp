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
