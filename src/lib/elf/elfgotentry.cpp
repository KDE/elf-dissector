/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "elfgotentry.h"
#include "elfgotsection.h"
#include "elffile.h"

ElfGotEntry::ElfGotEntry() :
    m_section(nullptr),
    m_index(0)
{
}

ElfGotEntry::ElfGotEntry(ElfGotSection* section, uint64_t index) :
    m_section(section),
    m_index(index)
{
}

ElfGotSection* ElfGotEntry::section() const
{
    return m_section;
}

uint64_t ElfGotEntry::index() const
{
    return m_index;
}

uint64_t ElfGotEntry::address() const
{
    return m_section->header()->virtualAddress() + m_index * m_section->file()->addressSize();
}

ElfRelocationEntry* ElfGotEntry::relocation() const
{
    return m_section->file()->reverseRelocator()->find(address());
}
