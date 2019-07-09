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

#ifndef ELF_SEGMENTHEADER_IMPL_H
#define ELF_SEGMENTHEADER_IMPL_H

#include "elfsegmentheader.h"

template <typename T>
class ElfSegmentHeaderImpl : public ElfSegmentHeader
{
public:
    explicit ElfSegmentHeaderImpl(ElfFile *file, uint16_t segmentIndex) :
        ElfSegmentHeader(file),
        m_hdr(reinterpret_cast<const T*>(file->rawData() + file->header()->programHeaderTableOffset() + segmentIndex * file->header()->programHeaderEntrySize()))
    {
    }

    ~ElfSegmentHeaderImpl() = default;

    uint32_t type() const final override { return m_hdr->p_type; }
    uint32_t flags() const final override { return m_hdr->p_flags; }
    uint64_t offset() const final override { return m_hdr->p_offset; }
    uint64_t virtualAddress() const final override { return m_hdr->p_vaddr; }
    uint64_t physicalAddress() const final override { return m_hdr->p_paddr; }
    uint64_t fileSize() const final override { return m_hdr->p_filesz; }
    uint64_t memorySize() const final override { return m_hdr->p_memsz; }
    uint64_t alignment() const final override { return m_hdr->p_align; }

private:
    const T* m_hdr = 0;
};

#endif
