/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "elfsymboltablesection.h"
#include "elfsectionheader.h"

#include <elf.h>

ElfSymbolTableSection::ElfSymbolTableSection(ElfFile* file, ElfSectionHeader *shdr): ElfSection(file, shdr)
{
    m_entries.reserve(header()->entryCount());
    m_entriesByValue.reserve(header()->entryCount());

    const uint64_t entryCount = header()->entryCount();
    for (uint64_t i = 0; i < entryCount; ++i) {
        m_entries.push_back(ElfSymbolTableEntry(this, i));

        const auto entry = m_entries.data() + i;
        if (entry->size() == 0 || entry->value() == 0) {
            continue;
        }
        m_entriesByValue.push_back(entry);
    }

    std::sort(m_entriesByValue.begin(), m_entriesByValue.end(), [](auto *lhs, auto *rhs) {
        return lhs->value() < rhs->value();
    });
}

ElfSymbolTableSection::~ElfSymbolTableSection() = default;

ElfSymbolTableEntry* ElfSymbolTableSection::entry(uint32_t index) const
{
    return const_cast<ElfSymbolTableEntry*>(m_entries.data() + index);
}

int ElfSymbolTableSection::exportCount() const
{
    int count = 0;
    for (const auto &entry : m_entries) {
        if (entry.bindType() == STB_GLOBAL && entry.size() > 0)
            ++count;
    }
    return count;
}

int ElfSymbolTableSection::importCount() const
{
    int count = 0;
    for (const auto &entry : m_entries) {
        if (entry.bindType() == STB_GLOBAL && entry.size() == 0)
            ++count;
    }
    return count;
}

ElfSymbolTableEntry* ElfSymbolTableSection::entryWithValue(uint64_t value) const
{
    if (value == 0)
        return nullptr;

    const auto it = std::lower_bound(m_entriesByValue.begin(), m_entriesByValue.end(), value, [](auto *lhs, uint64_t rhs) {
        return lhs->value() < rhs;
    });
    if (it != m_entriesByValue.end() && (*it)->value() == value) {
        return (*it);
    }
    return nullptr;
}


ElfSymbolTableEntry* ElfSymbolTableSection::entryContainingValue(uint64_t value) const
{
    if (value == 0)
        return nullptr;

    auto it = std::lower_bound(m_entriesByValue.begin(), m_entriesByValue.end(), value, [](auto *lhs, uint64_t rhs) {
        return lhs->value() < rhs;
    });
    if (it == m_entriesByValue.end()) { --it; }

    while (it != m_entriesByValue.end() && value < (*it)->value() + (*it)->size()) {
        if ((*it)->value() <= value) {
            return *it;
        }

        if (it == m_entriesByValue.begin()) {
            return nullptr;
        }
        --it;
    }

    return nullptr;
}
