/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFSEGMENTHEADER_H
#define ELFSEGMENTHEADER_H

#include <cstdint>

class ElfFile;

/** Represents a program segment header. */
class ElfSegmentHeader
{
public:
    ElfSegmentHeader(const ElfSegmentHeader &other) = delete;
    virtual ~ElfSegmentHeader();

    ElfFile* file() const;

    virtual uint32_t type() const = 0;
    virtual uint32_t flags() const = 0;
    virtual uint64_t offset() const = 0;
    virtual uint64_t virtualAddress() const = 0;
    virtual uint64_t physicalAddress() const = 0;
    virtual uint64_t fileSize() const = 0;
    virtual uint64_t memorySize() const = 0;
    virtual uint64_t alignment() const = 0;

protected:
    explicit ElfSegmentHeader(ElfFile *file);

private:
    ElfFile* const m_file;

};

#endif // ELFSEGMENTHEADER_H
