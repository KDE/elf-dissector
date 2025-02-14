/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
