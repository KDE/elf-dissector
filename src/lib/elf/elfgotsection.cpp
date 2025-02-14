/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "elfgotsection.h"
#include "elffile.h"

ElfGotSection::ElfGotSection(ElfFile* file, ElfSectionHeader* shdr) :
    ElfSection(file, shdr)
{
    const auto count = shdr->size() / file->addressSize();
    m_entries.reserve(count);
    for (uint64_t i = 0; i < count; ++i) {
        m_entries.push_back(ElfGotEntry(this, i));
    }
}

ElfGotSection::~ElfGotSection() = default;

ElfGotEntry* ElfGotSection::entry(uint64_t index) const
{
    return const_cast<ElfGotEntry*>(m_entries.data() + index);
}

uint64_t ElfGotSection::entryCount() const
{
    return header()->size() / file()->addressSize();
}
