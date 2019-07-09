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

#ifndef ELFPRINTER_H
#define ELFPRINTER_H

#include <cstdint>

class QByteArray;

/** Pretty printer functions for values related to ELF files or generic section information. */
namespace ElfPrinter
{
    /** File type. */
    QByteArray fileType(uint16_t fileType);
    /** Machine type of a file. */
    QByteArray machine(uint16_t machineType);
    /** OS ABI. */
    QByteArray osAbi(uint8_t abi);

    /** Section type. */
    QByteArray sectionType(uint32_t sectionType);
    /** Section flags. */
    QByteArray sectionFlags(uint64_t flags);

    /** Segment type. */
    QByteArray segmentType(uint32_t segmentType);
    /** Segment flags. */
    QByteArray segmentFlags(uint32_t flags);
}

#endif // ELFPRINTER_H
