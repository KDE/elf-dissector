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
