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

#include "elfgnudebuglinksection.h"

#include <QByteArray>

#include <cassert>

/*
 * Format, according to GDB manual:
 * - A filename, with any leading directory components removed, followed by a zero byte,
 * - zero to three bytes of padding, as needed to reach the next four-byte boundary within the section, and
 * - a four-byte CRC checksum, stored in the same endianness used for the executable file itself.
 * The checksum is computed on the debugging information file's full contents by the function given below, passing zero as the crc argument.
 */
ElfGnuDebugLinkSection::ElfGnuDebugLinkSection(ElfFile* file, ElfSectionHeader* shdr): ElfSection(file, shdr)
{
    assert(shdr->size() > 6);
}

ElfGnuDebugLinkSection::~ElfGnuDebugLinkSection()
{
}

QByteArray ElfGnuDebugLinkSection::fileName() const
{
    auto len = strnlen(reinterpret_cast<const char*>(rawData()), header()->size() - 4);
    return QByteArray::fromRawData(reinterpret_cast<const char*>(rawData()), len);
}

uint32_t ElfGnuDebugLinkSection::crc() const
{
    return *reinterpret_cast<uint32_t*>(rawData() + header()->size() - sizeof(uint32_t));
}
