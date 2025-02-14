/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "elfsectionheader.h"
#include "elffile.h"
#include "elfheader.h"
#include "elfstringtablesection.h"

#include <cassert>

ElfSectionHeader::ElfSectionHeader(ElfFile* file, uint16_t sectionIndex) :
    m_file(file), m_sectionIndex(sectionIndex)
{
    assert(file);
}

ElfSectionHeader::~ElfSectionHeader() = default;

uint16_t ElfSectionHeader::sectionIndex() const
{
    return m_sectionIndex;
}

uint64_t ElfSectionHeader::headerOffset() const
{
    return m_file->header()->sectionHeaderTableOffset() + sectionIndex() * m_file->header()->sectionHeaderEntrySize();
}

const char* ElfSectionHeader::name() const
{
    auto strTab = m_file->section<ElfStringTableSection>(m_file->header()->stringTableSectionHeader());
    if (!strTab)
        return nullptr;
    return strTab->string(nameIndex());
}

bool ElfSectionHeader::isDebugInformation() const
{
    return strncmp(name(), ".debug", 6) == 0 || strncmp(name(), ".gdb", 4) == 0;
}

uint64_t ElfSectionHeader::entryCount() const
{
    if (entrySize() <= 0)
        return 0;
    return size() / entrySize();
}

ElfFile* ElfSectionHeader::file() const
{
    return m_file;
}
