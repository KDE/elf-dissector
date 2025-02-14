/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
