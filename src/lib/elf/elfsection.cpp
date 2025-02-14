/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
