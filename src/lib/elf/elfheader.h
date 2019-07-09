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

#ifndef ELFHEADER_H
#define ELFHEADER_H

#include <cstdint>

/** Size-independent adaptor to ElfXX_Ehdr. */
class ElfHeader
{
public:
    ElfHeader() = default;
    ElfHeader(const ElfHeader&) = delete;
    virtual ~ElfHeader();

    ElfHeader& operator=(const ElfHeader&) = delete;

    virtual uint16_t type() const = 0;
    virtual uint16_t machine() const = 0;
//     virtual uint32_t version() const = 0;
    virtual uint64_t entryPoint() const = 0;
    virtual uint64_t programHeaderTableOffset() const = 0;
    virtual uint64_t sectionHeaderTableOffset() const = 0;
    virtual uint32_t flags() const = 0;
//     virtual uint16_t headerSize() const = 0;
    virtual uint16_t programHeaderEntrySize() const = 0;
    virtual uint16_t programHeaderCount() const = 0;
    virtual uint16_t sectionHeaderEntrySize() const = 0;
    virtual uint16_t sectionHeaderCount() const = 0;
    virtual uint16_t stringTableSectionHeader() const = 0;
};

template <typename T>
class ElfHeaderImpl : public ElfHeader
{
public:
    explicit inline ElfHeaderImpl(unsigned char *data) : m_hdr(reinterpret_cast<T*>(data)) {}

    inline uint16_t type() const override { return m_hdr->e_type; }
    inline uint16_t machine() const override { return m_hdr->e_machine; }
    inline uint64_t entryPoint() const override { return m_hdr->e_entry; }
    inline uint64_t programHeaderTableOffset() const override { return m_hdr->e_phoff; }
    inline uint64_t sectionHeaderTableOffset() const override { return m_hdr->e_shoff; }
    inline uint32_t flags() const override { return m_hdr->e_flags; }
    inline  uint16_t programHeaderCount() const override { return m_hdr->e_phnum; }
    inline uint16_t programHeaderEntrySize() const override { return m_hdr->e_phentsize; }
    inline uint16_t sectionHeaderEntrySize() const override { return m_hdr->e_shentsize; }
    inline uint16_t sectionHeaderCount() const override { return m_hdr->e_shnum; }
    inline uint16_t stringTableSectionHeader() const override { return m_hdr->e_shstrndx; }

private:
    T* m_hdr;
};

#endif // ELFHEADER_H
