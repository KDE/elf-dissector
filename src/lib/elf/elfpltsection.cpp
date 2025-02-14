/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "elfpltsection.h"
#include "elffile.h"
#include "elfgotsection.h"

#include <elf.h>

ElfPltSection::ElfPltSection(ElfFile* file, ElfSectionHeader* shdr):
    ElfSection(file, shdr),
    m_gotSection(nullptr)
{
    m_entries.reserve(shdr->entryCount());
    for (uint64_t i = 0; i < shdr->entryCount(); ++i)
        m_entries.push_back(ElfPltEntry(this, i));
}

ElfPltSection::~ElfPltSection() = default;

ElfPltEntry* ElfPltSection::entry(uint64_t index) const
{
    return const_cast<ElfPltEntry*>(m_entries.data() + index);
}

ElfGotSection* ElfPltSection::gotSection() const
{
    if (!m_gotSection) {
        const auto gotAddr = m_file->dynamicSection()->entryWithTag(DT_PLTGOT)->pointer();
        const auto gotIdx = m_file->indexOfSectionWithVirtualAddress(gotAddr);
        m_gotSection = m_file->section<ElfGotSection>(gotIdx);
    }
    return m_gotSection;
}
