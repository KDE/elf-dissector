/*
    Copyright (C) 2013-2014 Volker Krause <vkrause@kde.org>

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

#ifndef ELFSECTIONHEADER_H
#define ELFSECTIONHEADER_H

#include <cstdint>

class ElfFile;

/** Size-independent adapter to ElfXX_Shdr. */
class ElfSectionHeader
{
public:
    ElfSectionHeader(const ElfSectionHeader &other) = delete;
    virtual ~ElfSectionHeader();
    ElfSectionHeader& operator=(const ElfSectionHeader &other) = delete;

    uint16_t sectionIndex() const;
    /** The location of the header (not the section it describes) in the ELF file. */
    uint64_t headerOffset() const;

    /** String index of the section name. */
    virtual uint32_t nameIndex() const = 0;
    virtual uint32_t type() const = 0;
    virtual uint64_t flags() const = 0;
    virtual uint64_t virtualAddress() const = 0;
    /** The location of the section (not this header) in the ELF file. */
    virtual uint64_t sectionOffset() const = 0;
    virtual uint64_t size() const = 0;
    virtual uint32_t link() const = 0;
    virtual uint32_t info() const = 0;
    virtual uint64_t alignment() const = 0;
    virtual uint64_t entrySize() const = 0;

    /** Section name. */
    const char* name() const;
    /** Returns @c true if this section is related to debug information in some way. */
    bool isDebugInformation() const;

    /** Returns the amount of entries in this section if section is an array. */
    uint64_t entryCount() const;

    ElfFile* file() const;

protected:
    explicit ElfSectionHeader(ElfFile *file, uint16_t sectionIndex);

private:
    ElfFile *m_file = nullptr;
    uint16_t m_sectionIndex = 0;
};

#endif // ELFSECTIONHEADER_H
