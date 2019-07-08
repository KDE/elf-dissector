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
