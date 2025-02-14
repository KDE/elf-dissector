/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "elfgnusymbolversiontable.h"

#include <elf.h>

#include <cassert>
#include <type_traits>

static_assert(std::is_same<Elf32_Versym, Elf64_Versym>::value, "Elfxx_Versym changed");
static_assert(std::is_same<Elf32_Versym, uint16_t>::value, "Elfxx_Versym changed");


ElfGNUSymbolVersionTable::ElfGNUSymbolVersionTable(ElfFile* file, ElfSectionHeader* shdr):
    ElfSection(file, shdr)
{
}

ElfGNUSymbolVersionTable::~ElfGNUSymbolVersionTable() = default;

uint16_t ElfGNUSymbolVersionTable::versionIndex(uint32_t index) const
{
    assert(index < header()->entryCount());
    return *(reinterpret_cast<const uint16_t*>(rawData()) + index) & 0x7FFF;
}

bool ElfGNUSymbolVersionTable::isHidden(uint32_t index) const
{
    assert(index < header()->entryCount());
    return *(reinterpret_cast<const uint16_t*>(rawData()) + index) & 0x8000;
}
