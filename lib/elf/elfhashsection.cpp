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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "elfhashsection.h"
#include "elfsymboltableentry.h"
#include "elfsymboltablesection.h"

#include <elf.h>
#include <cassert>

ElfHashSection::ElfHashSection(ElfFile* file, ElfSectionHeader* shdr) :
    ElfSection(file, shdr)
{
}

ElfHashSection::~ElfHashSection() = default;

uint32_t ElfHashSection::bucketCount() const
{
    return *reinterpret_cast<const uint32_t*>(rawData());
}

uint32_t ElfHashSection::bucket(uint32_t index) const
{
    return *(reinterpret_cast<const uint32_t*>(rawData()) + 2 + index);
}

uint32_t ElfHashSection::chainCount() const
{
    return *(reinterpret_cast<const uint32_t*>(rawData()) + 1);
}

uint32_t ElfHashSection::chain(uint32_t index) const
{
    return *(reinterpret_cast<const uint32_t*>(rawData()) + 2 + bucketCount() + index);
}

uint32_t ElfHashSection::elfHash(const char* name)
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

ElfSymbolTableEntry* ElfHashSection::lookup(const char* name)
{
    const auto x = elfHash(name);

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

QVector<uint32_t> ElfHashSection::histogram() const
{
    QVector<uint32_t> hist;
    for (uint i = 0; i < bucketCount(); ++i) {
        int count = 0;

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
