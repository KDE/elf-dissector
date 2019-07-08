/*
    Copyright (C) 2015 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "elfgnuhashsection.h"
#include "elfsymboltablesection.h"
#include "elffile.h"


#include <cassert>

ElfGnuHashSection::ElfGnuHashSection(ElfFile* file, ElfSectionHeader* shdr):
    ElfHashSection(file, shdr)
{
    // must be a power of two
    assert ((maskWordsCount() & (maskWordsCount() - 1)) == 0);
}

ElfGnuHashSection::~ElfGnuHashSection() = default;

uint32_t ElfGnuHashSection::bucketCount() const
{
    return *reinterpret_cast<const uint32_t*>(rawData());
}

uint32_t ElfGnuHashSection::bucket(uint32_t index) const
{
    return *(reinterpret_cast<const uint32_t*>(rawData() + maskWordsCount() * file()->addressSize()) + 4 + index);
}

uint32_t ElfGnuHashSection::chainCount() const
{
    const auto symTab = linkedSection<ElfSymbolTableSection>();
    assert(symTab);
    return symTab->header()->entryCount() - symbolIndex();
}

uint32_t* ElfGnuHashSection::value(uint32_t index) const
{
    return const_cast<uint32_t*>(reinterpret_cast<const uint32_t*>(rawData() + maskWordsCount() * file()->addressSize()) + 4 + bucketCount() + index - symbolIndex());
}

uint32_t ElfGnuHashSection::symbolIndex() const
{
    return *(reinterpret_cast<const uint32_t*>(rawData()) + 1);
}

uint32_t ElfGnuHashSection::maskWordsCount() const
{
    return *(reinterpret_cast<const uint32_t*>(rawData()) + 2);
}

uint32_t ElfGnuHashSection::shift2() const
{
    return *(reinterpret_cast<const uint32_t*>(rawData()) + 3);
}

uint32_t ElfGnuHashSection::hash(const char* name)
{
    uint32_t h = 5381;

    for (unsigned char c = *name; c != '\0'; c = *++name)
        h = h * 33 + c;

    return h;
}

uint64_t ElfGnuHashSection::filterMask(uint32_t index) const
{
    if (file()->addressSize() == 8)
        return *(reinterpret_cast<const uint64_t*>(rawData()) + 2 + index);
    return *(reinterpret_cast<const uint32_t*>(rawData()) + 4 + index);
}

ElfSymbolTableEntry* ElfGnuHashSection::lookup(const char* name) const
{
    auto h1 = hash(name);

    {
        const uint32_t h2 = h1 >> shift2();
        const uint32_t c = file()->addressSize() * 8;
        const uint32_t n = (h1 / c) & (maskWordsCount() - 1);

        const uint32_t hashbit1 = h1 & (c - 1);
        const uint32_t hashbit2 = h2 & (c - 1);

        const auto bitmask = filterMask(n);
        if (((bitmask >> hashbit1) & (bitmask >> hashbit2) & 1) == 0)
            return nullptr;
    }

    auto n = bucket(h1 % bucketCount());
    if (n == 0)
        return nullptr;

    const auto symTab = linkedSection<ElfSymbolTableSection>();
    assert(symTab);
    auto hashValue = value(n);

    for (h1 &= ~1; true; ++n) {
        const auto entry = symTab->entry(n);
        const auto h2 = *hashValue++;
        if ((h1 == (h2 & ~1)) && strcmp(name, entry->name()) == 0)
            return entry;
        if (h2 & 1)
            break;
    }

    return nullptr;
}

QVector<uint32_t> ElfGnuHashSection::histogram() const
{
    QVector<uint32_t> hist;
    for (uint i = 0; i < bucketCount(); ++i) {
        int count = 0;

        const auto n = bucket(i);
        auto hashValue = value(n);

        while (!(*hashValue++ & 1))
            ++count;

        hist.resize(std::max(hist.size(), count + 1));
        hist[count]++;
    }
    return hist;
}

double ElfGnuHashSection::averagePrefixLength() const
{
    int sum = 0;
    int count = 0;

    const auto symTab = linkedSection<ElfSymbolTableSection>();
    assert(symTab);

    for (uint i = 0; i < bucketCount(); ++i) {
        auto n1 = bucket(i);
        auto hashValue1 = value(n1);
        while (!(*hashValue1 & 1)) {
            const auto entry1 = symTab->entry(n1);

            auto n2 = n1 + 1;
            auto hashValue2 = hashValue1 + 1;
            while (!(*hashValue2 & 1)) {
                const auto entry2 = symTab->entry(n2);
                sum += commonPrefixLength(entry1->name(), entry2->name());
                ++count;
                ++hashValue2;
            }
            ++hashValue1;
            ++n1;
        }
    }

    return count > 0 ? (double)sum / (double)count : 0.0;
}
