/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELFSECTIONHEADER_IMPL_H
#define ELFSECTIONHEADER_IMPL_H

#include "elfsectionheader.h"
#include "elffile.h"

template <typename T>
class ElfSectionHeaderImpl : public ElfSectionHeader
{
public:
    inline ElfSectionHeaderImpl(ElfFile *file, uint16_t sectionIndex) :
    ElfSectionHeader(file, sectionIndex),
    m_hdr(reinterpret_cast<const T*>(file->rawData() + headerOffset()))
    {
    }

    inline uint32_t nameIndex() const override { return m_hdr->sh_name; }
    inline uint32_t type() const override { return m_hdr->sh_type; }
    inline uint64_t flags() const override { return m_hdr->sh_flags; }
    inline uint64_t virtualAddress() const override { return m_hdr->sh_addr; }
    inline uint64_t sectionOffset() const override { return m_hdr->sh_offset; }
    inline uint64_t size() const override { return m_hdr->sh_size; }
    inline uint32_t link() const override { return m_hdr->sh_link; }
    inline uint32_t info() const override { return m_hdr->sh_info; }
    inline uint64_t alignment() const override { return m_hdr->sh_addralign; }
    inline uint64_t entrySize() const override { return m_hdr->sh_entsize; }

private:
    const T* m_hdr = 0;
};

#endif // ELFSECTIONHEADER_IMPL_H
