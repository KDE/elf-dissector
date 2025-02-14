/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "elfpltentry.h"
#include "elfpltsection.h"
#include "elfgotsection.h"
#include "elffile.h"
#include "elfheader.h"

#include <elf.h>

ElfPltEntry::ElfPltEntry(const ElfPltEntry&) = default;

ElfPltEntry::ElfPltEntry(ElfPltSection* section, uint64_t index) :
    m_section(section),
    m_index(index)
{
}

ElfPltEntry& ElfPltEntry::operator=(const ElfPltEntry&) = default;

ElfPltSection* ElfPltEntry::section() const
{
    return m_section;
}

uint64_t ElfPltEntry::index() const
{
    return m_index;
}

const uchar* ElfPltEntry::rawData() const
{
    return m_section->rawData() + m_section->header()->entrySize() * m_index;
}

uint64_t ElfPltEntry::size() const
{
    return m_section->header()->entrySize();
}

ElfGotEntry* ElfPltEntry::gotEntry() const
{
    if (!m_index)
        return nullptr;

    // see i386/x86_64 psABI documentation for content of the first entry
    // for AArch64 this is experimentally determined
    if (m_section->file()->header()->machine() == EM_AARCH64) {
        return m_section->gotSection()->entry(m_index + 1);
    } else {
        return m_section->gotSection()->entry(m_index + 2);
    }
}
