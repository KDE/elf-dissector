/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "elfrelocationsection.h"
#include "elfrelocationentry.h"
#include "elffile.h"

#include <elf.h>

ElfRelocationSection::ElfRelocationSection(ElfFile* file, ElfSectionHeader* shdr) :
    ElfSection(file, shdr)
{
    const auto size = shdr->entryCount();
    const auto withAddend = shdr->type() == SHT_RELA;
    m_entries.reserve(size);
    for (uint i = 0; i < size; ++i)
        m_entries.push_back(ElfRelocationEntry(this, i, withAddend));
}

ElfRelocationSection::~ElfRelocationSection() = default;

ElfRelocationEntry* ElfRelocationSection::entry(uint32_t index) const
{
    return const_cast<ElfRelocationEntry*>(m_entries.data() + index);
}
