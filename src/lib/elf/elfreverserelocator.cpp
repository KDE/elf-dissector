/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "elfreverserelocator.h"
#include "elfrelocationsection.h"

#include <cassert>

int ElfReverseRelocator::size() const
{
    indexRelocations();
    return m_relocations.size();
}

ElfRelocationEntry* ElfReverseRelocator::find(uint64_t vaddr) const
{
    indexRelocations();

    const auto it = std::lower_bound(m_relocations.cbegin(), m_relocations.cend(), vaddr, [](ElfRelocationEntry *entry, uint64_t vaddr) {
        return entry->offset() < vaddr;
    });

    if (it == m_relocations.cend() || (*it)->offset() != vaddr)
        return nullptr;

    return *it;
}

int ElfReverseRelocator::relocationCount(uint64_t beginVAddr, uint64_t length) const
{
    indexRelocations();

    const auto beginIt = std::lower_bound(m_relocations.cbegin(), m_relocations.cend(), beginVAddr, [](ElfRelocationEntry *entry, uint64_t vaddr) {
        return entry->offset() < vaddr;
    });

    if (beginIt == m_relocations.cend())
        return 0;

    const auto endIt = std::lower_bound(m_relocations.cbegin(), m_relocations.cend(), beginVAddr + length, [](ElfRelocationEntry *entry, uint64_t vaddr) {
        return entry->offset() < vaddr;
    });

    return std::distance(beginIt, endIt);
}

void ElfReverseRelocator::addRelocationSection(ElfRelocationSection* section)
{
    assert(m_relocations.isEmpty());
    m_relocSections.push_back(section);
}

void ElfReverseRelocator::indexRelocations() const
{
    if (!m_relocations.isEmpty())
        return;

    int totalSize = 0;
    std::for_each(m_relocSections.constBegin(), m_relocSections.constEnd(), [&totalSize](ElfRelocationSection* section) {
        totalSize += section->header()->entryCount();
    });

    m_relocations.resize(totalSize);
    auto oit = m_relocations.begin();
    for (const auto sec : m_relocSections) {
        for (uint64_t i = 0; i < sec->header()->entryCount(); ++i) {
            *oit++ = sec->entry(i);
        }
    }

    std::sort(m_relocations.begin(), m_relocations.end(), [](ElfRelocationEntry *lhs, ElfRelocationEntry *rhs) {
        return lhs->offset() < rhs->offset();
    });
}
