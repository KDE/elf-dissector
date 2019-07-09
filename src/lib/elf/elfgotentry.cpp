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
