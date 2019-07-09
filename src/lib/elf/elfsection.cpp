/*
    Copyright (C) 2013-2014 Volker Krause <vkrause@kde.org>

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

#include "elfsection.h"
#include "elffile.h"

#include <cassert>

ElfSection::ElfSection(ElfFile* file, ElfSectionHeader *shdr) :
    m_file(file),
    m_sectionHeader(shdr),
    m_linkedSection(nullptr)
{
}

ElfSection::~ElfSection()
{
}

void ElfSection::setLinkedSection(ElfSection* linkedSection)
{
    m_linkedSection = linkedSection;
}

uint64_t ElfSection::size() const
{
    return m_sectionHeader->size();
}

unsigned char* ElfSection::rawData() const
{
    return m_file->rawData() + m_sectionHeader->sectionOffset();
}

ElfFile* ElfSection::file() const
{
    return m_file;
}

ElfSectionHeader* ElfSection::header() const
{
    return m_sectionHeader;
}
