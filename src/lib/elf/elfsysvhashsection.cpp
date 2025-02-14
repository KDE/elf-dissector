/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "elfsysvhashsection.h"
#include "elfsymboltablesection.h"

#include <elf.h>
#include <cassert>

ElfSysvHashSection::ElfSysvHashSection(ElfFile* file, ElfSectionHeader* shdr):
    ElfHashSection(file, shdr)
{
}

ElfSysvHashSection::~ElfSysvHashSection() = default;

uint32_t ElfSysvHashSection::bucketCount() const
{
    return *reinterpret_cast<const uint32_t*>(rawData());
}

uint32_t ElfSysvHashSection::bucket(uint32_t index) const
{
    return *(reinterpret_cast<const uint32_t*>(rawData()) + 2 + index);
}

uint32_t ElfSysvHashSection::chainCount() const
{
    return *(reinterpret_cast<const uint32_t*>(rawData()) + 1);
}

uint32_t ElfSysvHashSection::chain(uint32_t index) const
{
    return *(reinterpret_cast<const uint32_t*>(rawData()) + 2 + bucketCount() + index);
}

uint32_t ElfSysvHashSection::hash(const char* name)
{
    unsigned long h = 0, g;
    while (*name)
    {
        h = (h << 4) + *name++;
        if ((g = h & 0xf0000000))
            h ^= g >> 24;
        h &= ~g;
    }
    return h;
}

ElfSymbolTableEntry* ElfSysvHashSection::lookup(const char* name) const
{
    const auto x = hash(name);

    const auto symTab = linkedSection<ElfSymbolTableSection>();
    assert(symTab);
    auto y = bucket(x % bucketCount());
    while (y != STN_UNDEF) {
        const auto entry = symTab->entry(y);
        if (strcmp(entry->name(), name) == 0)
            return entry;
        y = chain(y);
    }

    return nullptr;
}

QList<uint32_t> ElfSysvHashSection::histogram() const
{
    QList<uint32_t> hist;
    for (uint i = 0; i < bucketCount(); ++i) {
        qsizetype count = 0;

        auto y = bucket(i);
        while (y != STN_UNDEF) {
            ++count;
            y = chain(y);
        }

        hist.resize(std::max(hist.size(), count + 1));
        hist[count]++;
    }
    return hist;
}

double ElfSysvHashSection::averagePrefixLength() const
{
    int sum = 0;
    int count = 0;

    const auto symTab = linkedSection<ElfSymbolTableSection>();
    assert(symTab);

    for (uint i = 0; i < bucketCount(); ++i) {
        auto y1 = bucket(i);
        while (y1 != STN_UNDEF) {
            const auto entry1 = symTab->entry(y1);
            auto y2 = chain(y1);
            while (y2 != STN_UNDEF) {
                const auto entry2 = symTab->entry(y2);
                sum += commonPrefixLength(entry1->name(), entry2->name());
                ++count;
                y2 = chain(y2);
            }

            y1 = chain(y1);
        }
    }

    return count > 0 ? (double)sum / (double)count : 0.0;
}
